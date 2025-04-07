<?php

/*
Usage
$db = new database($dbname);

for selects:
$db->setQuery("SELECT * FROM `table`")
$resultArray = $db->loadResults();

$db->setQuery("SELECT * FROM `table` WHERE `primary_id` = '1'");
$resultObject = $db->loadResult();

for inserts:
$db->setQuery("INSERT INTO `table` (`id`, `example`) VALUES ('1', 'abc')");
if (!$db->runQuery()) {
	echo $db->getError();
}
*/


class database {
	var $_debug = 0;
	var $_sql = '';
	var $_error = '';
	var $_prefix = 'cc_';
	
	var $_numrows = 0;

    var $_DBhost = 'localhost';
	var $_DBuser = "root";
	var $_DBpass = "qazqazqaz1az3ar";
	var $_DBname = "cocolani_battle";
	var $url_root = "localhost/cocolani";
	
	public function __construct($dbname = 'cocolani_battle', $dbhost, $dbuser, $dbpsw, $urlroot) {

    		$this->_DBname = $dbname;

		if ($_SERVER["SERVER_ADDR"] == "127.0.0.1") {
    			$this->_DBuser = "root";
	 	  	$this->_DBpass = "";
	 		$this->url_root = "http://cocolani.localhost";
		} else {
			$this->_DBuser = $dbuser;
		  	$this->_DBpass = $dbpsw;
		  	$this->url_root = $urlroot;
		  	$this->_DBhost = $dbhost;
		}
		$this->_connection = mysql_connect($this->_DBhost, $this->_DBuser, $this->_DBpass) or die("Couldn't connect to MySQL");
		mysql_select_db($this->_DBname) or die("Select DB Error: ".mysql_error());

	}
	
	public function __destruct() {
		mysql_close($this->_connection);
	}

	function debug($debug_level) {
		$this->_debug = intval($debug_level);	
	}
	
	function setQuery($sql) {
		/* queries are given in the form of #__table need to replace that with the prefix */
		$this->_sql = str_replace('#__', $this->_prefix.'_', $sql);
	}
	
	function getQuery() {
		return "<pre>" . htmlspecialchars( $this->_sql) . "</pre>";
	}
	
	function runQuery($num_rows=0) {
		mysql_select_db($this->_DBname) or die("Select DB Error: ".mysql_error());
		
		$this->_numrows = 0;
		$result = mysql_query($this->_sql, $this->_connection);
		if ($this->_debug > 1) echo "<pre>" . htmlspecialchars( $this->_sql) . "</pre>";
		
		if (!$result) {
			$this->_error = mysql_error($this->_connection);
			if ($this->_debug) {
				echo 'Error: ' . $this->getQuery() . $this->_error;
			}
			return false;
		}
		if ($num_rows) {
			$this->_numrows = mysql_num_rows($result);
		}
		return $result;
	}
	 
	/* Retrieve Mysql insert id */
	function mysqlInsertID() {
    $insert_id = mysql_insert_id();
    return $insert_id;
	}

/* Escapes special characters while inserting to db */
	function db_input($string) {
      if (is_array($string)) {
          $retArray = array();
          foreach($string as $key => $value) {
              $value = (get_magic_quotes_gpc() ? stripslashes($value) : $value);
              $retArray[$key] = mysql_real_escape_string($value);
          }
          return $retArray;
      } else {
          $string = (get_magic_quotes_gpc() ? stripslashes($string) : $string);
		  return mysql_real_escape_string($string);

      }
	}


	function getError() {
		return $this->_error;
	}
	/* Load results into csv formatted string */
	function loadCsv() {
		if (!($res = $this->runQuery())) {
			return null;
		}

		$csv_string = '';
		while ($row = mysql_fetch_row($res)) {
			$line = '';
			foreach( $row as $value ) {                                            
				if ( ( !isset( $value ) ) || ( $value == "" ) ) {
					$value = ",";
				} else { 
					$value = $value. ",";
					$value = str_replace( '"' , '""' , $value );
				}
				$line .= $value;
			}
			$line = substr($line, 0, -1);
			$csv_string .= trim( $line ) . "\n";
		}
		$csv_string = str_replace( "\r" , "" , $csv_string );
			//$csv_string .= implode(",", $row) . "\n";
		mysql_free_result($res);
		return $csv_string;
	}
	
	/* Load multiple results */
	function loadResults($key='' ) {
		if (!($res = $this->runQuery())) {
			return null;
		}
		$array = array();
		while ($row = mysql_fetch_object($res)) {
			if ($key) {
				$array[strtolower($row->$key)] = $row;
			} else {
				$array[] = $row;
			}
		}
		mysql_free_result($res);
		return $array;
	}
	
	function loadResult() {
		if (!($res = $this->runQuery())) {
			if ($this->_debug) echo 'Error: ' . $this->_error;
			return null;
		}
		$row = mysql_fetch_object($res);
		mysql_free_result($res);
		return $row;
	}
	
	/* Load a result field into an array */
	function loadArray() {
		if (!($res = $this->runQuery())) {
			return null;
		}
		$array = array();
		while ($row = mysql_fetch_row($res)) {
			$array[] = $row[0];
		}
		mysql_free_result($res);
		return $array;
	}
	
	/* Load a row into an associative an array */
	function loadAssoc() {
		if (!($res = $this->runQuery())) {
			return null;
		}
		$row = mysql_fetch_assoc($res);
		mysql_free_result($res);
		return $row;
	}
	
	/* Return one field */
	function loadField() {
		if (!($res = $this->runQuery())) {
			return null;
		}
		while ($row = mysql_fetch_row($res)) {
			$field = $row[0];
		}
		mysql_free_result($res);
		return $field;
	}

}

  /*if ($_SERVER["SERVER_ADDR"] == '127.0.0.1') {
    $url_root = "http://cocolani.localhost";
  } else {
    $url_root = "http://dev.cocolani.com";
  }*/


?>
