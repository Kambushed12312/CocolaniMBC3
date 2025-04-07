<?php
  
  function _echo($s) {
    if ($_SERVER['REMOTE_ADDR'] == "123.243.173.68") echo $s;
  }
  // ---------------------------------------------------
  // Main object
  // ---------------------------------------------------
  class Main {
  
    protected $db_server, $db_user, $db_password, $db_name, $url_admin, $url_from_root, $tbl_prefix, $text_direction;
    public $url_root, $reg_url, $connLimit;
    
    // ---------------------------------------------------
    // Constructor
    public function __construct($tbl_prefix = '', $_db_server, $_db_user, $_db_password, $_db_name, $_url_admin, $_url_from_root) {

  		// For PHP_MAILER
    	require_once ("config_email.php");
      $this->mail_smtp = $mail_smtp;
  		$this->smtp_host = $smtp_host;
  		$this->smtp_secure = $smtp_secure;
  		$this->smtp_auth = $smtp_auth;
  		$this->smtp_user = $smtp_user;
  		$this->smtp_pass = $smtp_pass;
  		$this->tbl_prefix = $tbl_prefix;
      if ($_SERVER["SERVER_ADDR"] == '127.0.0.1' || $_SERVER["SERVER_ADDR"] == '74.208.185.21') {
        error_reporting(E_ALL);
        $this->debug = true;
      } else {
        // DO NOT CHANGE ERROR_REPORTING SWITCH.
		error_reporting(0);
        $this->debug = false;
      }
      
      $this->db_server = $_db_server;
      $this->db_user = $_db_user;
      $this->db_password = $_db_password;
      $this->db_name = $_db_name;
      $this->url_admin = $_url_admin;          
      $this->url_from_root = $_url_from_root;
      $this->tbl_prefix = $tbl_prefix;
      $this->news_per_page = 2;
      
      // connect to database
      mysql_connect($this->db_server,$this->db_user,$this->db_password) or die("Can't connect to database.");
      mysql_select_db($this->db_name) or die("No database selected.");
      mysql_query("set names utf8");
      
      // get url_root saved in def_settings table
      $query = $this->runSQL("SELECT * FROM `{$tbl_prefix}def_settings`");
      $row = mysql_fetch_object($query);
      $this->url_root = $row->base_url;
      $this->swf_url = $row->swf_url;
      $this->reg_url = $row->reg_url;
     
      $this->connLimit = $this->getConnLimit();
      
      $this->getLangs();

      
    } // __construct

	public function __destruct() {
		mysql_close();
	}
    
    function log($s, $filename_debug = 'debug.log') {      
      if (!$this->debug) return true;
      $f = fopen($filename_debug, 'a+');
      fputs($f, date('c').' '.$s."\n");      
    }

    // ---------------------------------------------------
    function getLangs() {
      global $tbl_prefix;
      // get all the available languages
      $sql = "SELECT * FROM {$tbl_prefix}def_lang ORDER BY `lang` LIMIT 0,1";
      $query = $this->runSQL($sql);
      $row = mysql_fetch_object($query);
      // create langs_available and langs arrays
      $this->langs_available = $this->langs = array($row->lang);
      // create lang_ids
      $this->lang_ids = array(0);
      // get default text direction
      $this->default_text_direction = $row->direction;
      $default_lang = $row->lang;      
      $sql = "SELECT * FROM {$tbl_prefix}extra_langs ORDER BY `lang`";
      $query = $this->runSQL($sql);
      while ($row = mysql_fetch_object($query)) {
        array_push($this->langs, $row->lang);
        array_push($this->lang_ids, $row->id);
        if ($row->active == 1) array_push($this->langs_available, $row->lang);
      }
    }
    
    function getLangDirection($lang) {
      global $tbl_prefix;
      $sql = "SELECT * FROM {$tbl_prefix}extra_langs WHERE lang='{$lang}'";      
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $direction = $row->direction;
      } else {
        $direction = 'ltr'; 
      }      
      return $direction;
    }
    
    
    
    // ---------------------------------------------------
    // Getters
    // ---------------------------------------------------
    function getRootURL() {
      return $this->url_root;
    }
    
    function get($s) {
      return $this->{$s};
    }
    
    function getTemplatePath() {
      global $tbl_prefix;
      $sql = "SELECT template_path FROM {$tbl_prefix}def_settings";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $template_path = $row->template_path;
      } else {
        $template_path = ''; 
      }      
      return $template_path;
    }

    // ---------------------------------------------------
    // Setters
    // ---------------------------------------------------
    
    // set text direction : 'left' or 'right'
    function setTextDirection($text_direction) {
      $this->text_direction = $text_direction;      
    }

    // ---------------------------------------------------
    // Database routines
    function runSQL($sql) {
      $query = mysql_query($sql) or die($sql."<br />".mysql_error());
      return $query;
    } // runSQL
    
    function getTableValue($get_field, $table_name, $where, $return_if_no_results = 0) {    
      $sql = "SELECT `{$get_field}` FROM `{$this->tbl_prefix}{$table_name}` WHERE {$where}";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        return $row->$get_field;
      } else
        return $return_if_no_results;
    } 
    
    

    // ---------------------------------------------------
    // returns 1 --- it's limited to one connection 
    // ---------------------------------------------------
    function getConnLimit() {
      $sql = "SELECT * FROM {$this->tbl_prefix}def_settings WHERE id=1";
      $query = $this->runSQL($sql);      
      if (mysql_num_rows($query) == 0) {
        return 1; 
      } else {
        $row = mysql_fetch_object($query);
        return $row->limit_one_connection;
      }
    }


    // ---------------------------------------------------
    // Get main menu
    // $lang: <empty>, _ae, etc. 
    // ---------------------------------------------------
    function get_main_menu($section, $lang) {
      global $data, $WHATS_NEW, $TRIBES, $DISCOVER, $MEMBERS, $PLAY;
      $root = ($lang == "" || $lang == "_gb") ? $this->url_root : $this->url_root."/".substr($lang, 1);
      
      $ar["li_class"] = "m1";
      $ar["a_selected"] = ($section == 1) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/whats-new";
      $ar["title"] = $WHATS_NEW; 
      array_push($data['main_menu'], $ar);
      
      $ar["li_class"] = "m2";
      $ar["a_selected"] = ($section == 2) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/tribes";
      $ar["title"] = $TRIBES; 
      array_push($data['main_menu'], $ar);
      
      $ar["li_class"] = "m3";
      $ar["a_selected"] = ($section == 3) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/discover";
      $ar["title"] = $DISCOVER; 
      array_push($data['main_menu'], $ar);
      
      $ar["li_class"] = "m4";
      $ar["a_selected"] = ($section == 4) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/members";
      $ar["title"] = $MEMBERS; 
      array_push($data['main_menu'], $ar);

      $ar["li_class"] = "m5";
      $ar["a_selected"] = ($section == 5) ? "class=\"selected\"" : "";            
      $ar["href"] = ($lang == "") ? $this->url_root."/play" : $this->url_root."/play?lang=".substr($lang, 1);
      $ar["title"] = $PLAY; 
      array_push($data['main_menu'], $ar);

    } // show_menu

    // ---------------------------------------------------
    // Show footer menu and copyright info, logo
    // $m = array('home', 'whats_new', 'tribes', 'discover', 'members', 'feedback', 'faq', 'terms-and-conditions', 'play', 'copyright', 'logo');
    // ---------------------------------------------------
    function show_footer_menu($section, $lang, $m = null) {
/*function footer_menu($menu=null) {   if(!isset($menu) || in_array('first', $menu)) echo '1'; }*/
      global $data, $HOME, $WHATS_NEW, $TRIBES, $DISCOVER, $MEMBERS, $FEEDBACK, $FAQ, $TERMS_CONDITIONS, $PLAY, $FOOTER_COPYRIGHT;      
      $root = ($lang == "" || $lang == "_gb") ? $this->url_root : $this->url_root."/".substr($lang, 1);
      $first_created = false;

      $menu = array(
        'home' => array('title' => $HOME, 'href' => '/', 'section' => 0), 
        'whats_new' => array('title' => $WHATS_NEW, 'href' => '/whats-new', 'section' => 1), 
        'tribes' => array('title' => $TRIBES, 'href' => '/tribes', 'section' => 2),
        'discover' => array('title' => $DISCOVER, 'href' => '/discover', 'section' => 3), 
        'members' => array('title' => $MEMBERS, 'href' => '/members', 'section' => 4),
        'feedback' => array('title' => $FEEDBACK, 'href' => '/feedback', 'section' => 6), 
        'faq' => array('title' => $FAQ, 'href' => 'faq', 'section' => 7),
        'terms-and-conditions' => array('title' => $TERMS_CONDITIONS, 'href' => '/terms-and-conditions', 'section' => 8), 
        'play' => array('title' => $PLAY, 'href' => '/play', 'section' => 0),
      );

      if (isset($m)) {
        $first_created = false;
        foreach($m as $m_element) {
          $ar["li_class"] = ($first_created == false) ? "class=\"first\"" : ""; 
          $first_created = true;
          $ar["a_selected"] = ($section == $menu[$m_element]['section']) ? "class=\"selected\"" : "";
          $ar["href"] = $root.$menu[$m_element]['href'];
          $ar["title"] = $menu[$m_element]['title'];
          array_push($data["footer_menu"], $ar); 
        }
      }
      
      if(!isset($m)) { // Home
        $ar["li_class"] = "class=\"first\""; $first_created = true;
        $ar["a_selected"] = "";
        $ar["href"] = $root."/";
        $ar["title"] = $HOME;
        array_push($data["footer_menu"], $ar); 
        
      // What's new
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 1) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/whats-new";
        $ar["title"] = $WHATS_NEW;
        array_push($data["footer_menu"], $ar);        

      // Tribes
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 2) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/tribes";
        $ar["title"] = $TRIBES;
        array_push($data["footer_menu"], $ar);        

      // Discover
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 3) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/discover";
        $ar["title"] = $DISCOVER;
        array_push($data["footer_menu"], $ar);        

      // Members
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 4) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/members";
        $ar["title"] = $MEMBERS;
        array_push($data["footer_menu"], $ar); 

      // Feedback
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 6) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/feedback";
        $ar["title"] = $FEEDBACK;
        array_push($data["footer_menu"], $ar); 

      // faq
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 7) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/faq";
        $ar["title"] = $FAQ;
        array_push($data["footer_menu"], $ar); 

      // terms-and-conditions
        $ar["li_class"] = (!$first_created) ? "class=\"first\"" : ""; $first_created = true;
        $ar["a_selected"] = ($section == 8) ? "class=\"selected\"" : "";
        $ar["href"] = $root."/terms-and-conditions";
        $ar["title"] = $TERMS_CONDITIONS;
        array_push($data["footer_menu"], $ar); 

      // play
        $ar["li_class"] = "";
        $ar["a_selected"] = "";
        $ar["href"] = $root."/play";
        $ar["title"] = $PLAY;
        array_push($data["footer_menu"], $ar); }
      
      //if(!isset($m) || in_array('copyright', $m)) { array_push($data["footer_copyright"], $FOOTER_COPYRIGHT); }
      //if(!isset($m) || in_array('logo', $m)) { $array_push($data["footer_logo_src"], $this->url_root."/images/bottom-logo.jpg"); }
       
    } // show_footer_menu

    function _show_footer_menu($section, $lang, $on_play_page = false, $m = null) {
      global $HOME, $WHATS_NEW, $TRIBES, $DISCOVER, $MEMBERS, $FEEDBACK, $FAQ, $TERMS_CONDITIONS, $PLAY, $FOOTER_COPYRIGHT;      
      $root = ($lang == "" || $lang == "_gb") ? $this->url_root : $this->url_root."/".substr($lang, 1);
    ?> 
    <ul>
      <li class="first" title="Home"><a href="<?php echo $root; ?>/"><?php echo $HOME; ?></a></li>
      <li title="<?php echo $WHATS_NEW; ?>"><a <?php if ($section == 1) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/whats-new" ><?php echo $WHATS_NEW; ?></a></li>
      <li title="<?php echo $TRIBES; ?>"><a <?php if ($section == 2) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/tribes"><?php echo $TRIBES; ?></a></li>
      <li title="<?php echo $DISCOVER; ?>"><a <?php if ($section == 3) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/discover"><?php echo $DISCOVER; ?></a></li>
      <li title="<?php echo $MEMBERS; ?>"><a <?php if ($section == 4) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/members"><?php echo $MEMBERS; ?></a></li>
      <?php if ($on_play_page == false) { ?><li title="<?php echo $FEEDBACK; ?>"><a <?php if ($section == 6) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/feedback"><?php echo $FEEDBACK; ?></a></li><?php } ?>
      <?php if ($on_play_page == false) { ?><li title="<?php echo $FAQ; ?>"><a <?php if ($section == 7) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/faq"><?php echo $FAQ; ?></a></li><?php } ?>
      <?php if ($on_play_page == false) { ?><li title="<?php echo $TERMS_CONDITIONS; ?>"><a <?php if ($section == 8) echo " class=\"selected\""; ?> href="<?php echo $root; ?>/terms-and-conditions"><?php echo $TERMS_CONDITIONS; ?></a></li><?php } ?>
      <?php if ($on_play_page == false) { ?><li title="<?php echo $PLAY; ?>"><a href="<?php echo $root; ?>/play"><?php echo $PLAY; ?></a></li><?php } ?>      
    </ul>
    <?php if ($on_play_page == false) { ?>
    <div style="margin-top: 5px;"><?php echo $FOOTER_COPYRIGHT; ?></div>
    <div style="margin-top: 5px;"><a href="/"><img src="<?php echo $this->url_root; ?>/images/bottom-logo.jpg" alt="" style="" /></a></div>
    <?php } ?>  
    <?php
    } // show_footer_menu


    function get_footer_menu($section, $lang) {
      global $data, $HOME, $WHATS_NEW, $TRIBES, $DISCOVER, $MEMBERS, $FEEDBACK, $FAQ, $TERMS_CONDITIONS, $PLAY, $FOOTER_COPYRIGHT;      
      $FAQ = "FAQ";
      $TERMS_CONDITIONS = "Terms And Conditions";
      $root = ($lang == "" || $lang == "_gb") ? $this->url_root : $this->url_root."/".substr($lang, 1);

      $ar["a_selected"] = "";
      $ar["href"] = $root;
      $ar["title"] = $HOME; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = ($section == 1) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/whats-new";
      $ar["title"] = $WHATS_NEW; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = ($section == 2) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/tribes";
      $ar["title"] = $TRIBES; 
      array_push($data["footer_menu"], $ar);
      
      $ar["a_selected"] = ($section == 3) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/discover";
      $ar["title"] = $DISCOVER; 
      array_push($data["footer_menu"], $ar);
      
      $ar["a_selected"] = ($section == 4) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/members";
      $ar["title"] = $MEMBERS; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = ($section == 6) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/feedback";
      $ar["title"] = $FEEDBACK; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = ($section == 7) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/faq";
      $ar["title"] = $FAQ; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = ($section == 8) ? "class=\"selected\"" : "";
      $ar["href"] = $root."/terms-and-conditions";
      $ar["title"] = $TERMS_CONDITIONS; 
      array_push($data["footer_menu"], $ar);

      $ar["a_selected"] = "";
      $ar["href"] = $root."/play";
      $ar["title"] = $PLAY; 
      array_push($data["footer_menu"], $ar);

    } // show_footer_menu

    
    // ---------------------------------------------------
    // Check whether my IP address is banned
    // ---------------------------------------------------
    function showCurrencies($name) { ?>
      <select name="<?php echo $name; ?>">
        <option>AUD</option>
        <option>USD</option>
        <option>EUR</option>
      </select>
    <?php 
    } // showCurrencies
    

    // ---------------------------------------------------
    // Get content
    // ---------------------------------------------------
    function getContent($id, $lang = "") {
    
      $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}page WHERE id={$id}");
      
      if (mysql_num_rows($query) == 0) return 'No content.'; else {
        $row = mysql_fetch_object($query);
        return htmlspecialchars_decode($row->{"content".$lang});
      }
      //$FEEDBACK_OPTION_1
      
    } // getContent($id)
    
   
    // ---------------------------------------------------
    // Get last news for the Front page, show only last $n_news records
    // ---------------------------------------------------
    function getLastNews($lang, $n_news = 2) {      
      global $data, $MORE, $READ_MORE_NEWS;
      $data['last_news'] = array();     
      $sql = "SELECT * FROM {$this->tbl_prefix}mod_news WHERE enabled=1 ORDER BY `news_date` DESC LIMIT 0, {$n_news}";      
      $query = $this->runSQL($sql);
      //$res = "";
      $root = ($lang == "" || $lang == "_gb") ? $this->url_root : $this->url_root."/".substr($lang, 1);      
      $cur_page = 1;
      $i = 0;      
      while ($row = mysql_fetch_object($query)) {
        $content = strip_tags(html_entity_decode($row->{"content".$lang}));
        if (strlen($content) > 38) $content = substr($content, 0, 38).'...';
        $ar["title"] = $row->{"title".$lang};
        $ar["href"] = "{$root}/whats-new?cur_page={$cur_page}#news_{$row->id}";
        //$res .= "<li><strong>{$row->{"title".$lang}}</strong><br />{$content} <a href=\"{$root}/whats-new?cur_page={$cur_page}#news_{$row->id}\">{$MORE}&nbsp;&gt;&gt;</a></li>";
        ++$i;
        if ($this->news_per_page == $i) {
          ++$cur_page;
          $i = 0;
        }    
        array_push($data['last_news'], $ar);
      }
      //$res .= "<li style=\"background-image: none;\"><a href=\"{$root}/whats-new\">{$READ_MORE_NEWS}</a></li>";      
      //return $res;      
    } // getLastNews($n_news = 2)
    
   
    // ---------------------------------------------------
    // Get all news in "What's new" page
    // ---------------------------------------------------
    function getAllLastNews($page, $selected_keyword, $rowsPerPage = 0) {      
      global $data, $lang, $FILLED_UNDER, $PAGES;
      
      $data["news"] = array();
      $news_per_page = 2;
      $sql = "SELECT * FROM {$this->tbl_prefix}mod_news_settings";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $news_per_page = $row->news_per_page;
      }      
      $rowsPerPage = ($rowsPerPage == 0) ? $news_per_page : $rowsPerPage;    
      $sql = "SELECT n.id, n.title{$lang} as title, date_format(n.news_date, '%M %d %Y') as news_date1, n.content{$lang} as content  
        FROM {$this->tbl_prefix}mod_news n
        LEFT JOIN {$this->tbl_prefix}mod_news_keywords mnk ON n.id = mnk.news_id
        LEFT JOIN {$this->tbl_prefix}keywords k ON mnk.keyword_id = k.id
        WHERE enabled=1".(($selected_keyword != '') ? " AND k.name{$lang}='{$selected_keyword}'" : "")." ORDER BY `news_date` DESC";      
      $query = $this->runSQL($sql);
      $numrows =  mysql_num_rows($query);
      // how many pages we have when using paging?
      $maxPage = ceil($numrows / $rowsPerPage);
      if ($page > $maxPage) $page = $maxPage;
      if ($maxPage > 1) {
        $sql .= " LIMIT " . (($page-1) * $rowsPerPage) . ", " . $rowsPerPage;        
        $query = $this->runSQL($sql);
      } else {
        $query = $this->runSQL($sql);
      }
      $query = $this->runSQL($sql);
      $res = "";
      while ($row = mysql_fetch_object($query)) {                     
        $ar["id"] = $row->id;
        $ar["title"] = $row->title;
        $ar["date"] = $row->news_date1;
        $ar["content"] = strip_tags(html_entity_decode($row->content));           
        //$res .= "<a name=\"news_{$row->id}\"></a>";
        //$res .= "<div class=\"news_box\">";
        //$res .= "<div class=\"caption\">{$row->title}</div><div class=\"date\">{$row->news_date1}</div><div style=\"clear: both;\"></div>";
        //$res .= "<div class=\"content\">{$content}</div>";
        // get keywords
        $sql = "SELECT k.name{$lang} as keyword_name
          FROM {$this->tbl_prefix}mod_news_keywords nk
          JOIN {$this->tbl_prefix}keywords k ON nk.keyword_id = k.id
          WHERE nk.news_id = {$row->id}
        ";        
        $query1 = $this->runSQL($sql);
        if (mysql_num_rows($query1) > 0) {
          $keys = array();
          //$res .= "<div class=\"tags\"> {$FILLED_UNDER} ";
          while ($row1 = mysql_fetch_object($query1)) {          
            $keys[] = "<a href=\"?keyword={$row1->keyword_name}\">{$row1->keyword_name}</a>";
          }
          $ar["tags"] = implode(", ", $keys);
          //$res .= "<span class=\"tags_txt\">".implode(", ", $keys)."</span>";
          //$res .= "</div>";
        }        
        //$res .= "</div>";
        array_push($data['news'], $ar);
      }      
      
      // pagination
      $data['news_pages'] = array();
      if ($maxPage > 1) {
        //$res .= "<div style=\"margin-left: 24px;\">{$PAGES} ";
        $base_url = (substr($lang, 0, 1) == "_") ? "/".substr($lang, 1) : "";        
        for ($i=1; $i<=$maxPage; $i++) {
          $url = "{$base_url}/whats-new?cur_page={$i}&amp;keyword={$selected_keyword}"; 
          if ($i != $page)
            $link = "<span style=\"padding: 2px 3px;\"><a href=\"{$url}\" style=\"color: #0000cc; text-decoration: none;\">{$i}</a></span>&nbsp;"; else
            $link = "<span style=\"padding: 2px 3px;\"><a href=\"{$url}\" style=\"color: black; text-decoration: none; font-weight: bold;\">{$i}</a></span>&nbsp;";
          $ar['link'] = $link;  
          array_push($data['news_pages'], $ar);   
        }
        //$res .= "</div>";
      }
      //return $res;
    } // getAllLastNews()
    

    function _getAllLastNews($page, $selected_keyword, $rowsPerPage = 0) {      
      global $lang, $FILLED_UNDER, $PAGES;
      $news_per_page = 2;
      $sql = "SELECT * FROM {$this->tbl_prefix}mod_news_settings";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $news_per_page = $row->news_per_page;
      }      
      $rowsPerPage = ($rowsPerPage == 0) ? $news_per_page : $rowsPerPage;    
      $sql = "SELECT n.id, n.title{$lang} as title, date_format(n.news_date, '%M %d %Y') as news_date1, n.content{$lang} as content  
        FROM {$this->tbl_prefix}mod_news n
        LEFT JOIN {$this->tbl_prefix}mod_news_keywords mnk ON n.id = mnk.news_id
        LEFT JOIN {$this->tbl_prefix}keywords k ON mnk.keyword_id = k.id
        WHERE enabled=1".(($selected_keyword != '') ? " AND k.name{$lang}='{$selected_keyword}'" : "")." ORDER BY `news_date` DESC";      
      $query = $this->runSQL($sql);
      $numrows =  mysql_num_rows($query);
      // how many pages we have when using paging?
      $maxPage = ceil($numrows / $rowsPerPage);
      if ($page > $maxPage) $page = $maxPage;
      if ($maxPage > 1) {
        $sql .= " LIMIT " . (($page-1) * $rowsPerPage) . ", " . $rowsPerPage;        
        $query = $this->runSQL($sql);
      } else {
        $query = $this->runSQL($sql);
      }
      $query = $this->runSQL($sql);
      $res = "";
      while ($row = mysql_fetch_object($query)) {            
        $content = strip_tags(html_entity_decode($row->content));  
        $res .= "<a name=\"news_{$row->id}\"></a>";
        $res .= "<div class=\"news_box\">";
        $res .= "<div class=\"caption\">{$row->title}</div><div class=\"date\">{$row->news_date1}</div><div style=\"clear: both;\"></div>";
        $res .= "<div class=\"content\">{$content}</div>";
        // get keywords
        $sql = "SELECT k.name{$lang} as keyword_name
          FROM {$this->tbl_prefix}mod_news_keywords nk
          JOIN {$this->tbl_prefix}keywords k ON nk.keyword_id = k.id
          WHERE nk.news_id = {$row->id}
        ";        
        $query1 = $this->runSQL($sql);
        if (mysql_num_rows($query1) > 0) {
          $keys = array();
          $res .= "<div class=\"tags\"> {$FILLED_UNDER} ";
          while ($row1 = mysql_fetch_object($query1)) {          
            $keys[] = "<a href=\"?keyword={$row1->keyword_name}\">{$row1->keyword_name}</a>";
          }
          $res .= "<span class=\"tags_txt\">".implode(", ", $keys)."</span>";
          $res .= "</div>";
        }        
        $res .= "</div>";
      }      
      // pagination
      if ($maxPage > 1) {
        $res .= "<div style=\"margin-left: 24px;\">{$PAGES} ";
        $base_url = (substr($lang, 0, 1) == "_") ? "/".substr($lang, 1) : "";        
        for ($i=1; $i<=$maxPage; $i++) {
          $url = "{$base_url}/whats-new?cur_page={$i}&amp;keyword={$selected_keyword}"; 
          if ($i != $page)
            $res .= "<span style=\"padding: 2px 3px;\"><a href=\"{$url}\" style=\"color: #0000cc; text-decoration: none;\">{$i}</a></span>&nbsp;"; else
            $res .= "<span style=\"padding: 2px 3px;\"><a href=\"{$url}\" style=\"color: black; text-decoration: none; font-weight: bold;\">{$i}</a></span>&nbsp;"; 
        }
        $res .= "</div>";
      }
      return $res;
    } // _getAllLastNews()


    // ---------------------------------------------------
    // Get all categories
    // ---------------------------------------------------
    function getAllCategories($selected_keyword = '') {      
      global $data, $lang;
      $data["news_categories"] = array();      
      $sql = "SELECT id, name{$lang} as `name` FROM {$this->tbl_prefix}keywords ORDER BY `name`";
      $query = $this->runSQL($sql);
      $tags = array();
      while ($row = mysql_fetch_object($query)) {
        array_push($tags, $row->name);
      }
      foreach($tags as $keyword) {
        $ar["href"] = "?keyword={$keyword}";
        $ar["a_selected"] = (trim($keyword) == trim($selected_keyword)) ? " class=\"selected\"" : "";
        $ar["news_keyword"] = $keyword;
        array_push($data["news_categories"], $ar);
      }
    } // getAllCategories()

    function _getAllCategories($selected_keyword = '') {      
      global $lang;      
      $sql = "SELECT id, name{$lang} as `name` FROM {$this->tbl_prefix}keywords ORDER BY `name`";
      $query = $this->runSQL($sql);
      $tags = array();
      while ($row = mysql_fetch_object($query)) {
        array_push($tags, $row->name);
      }
      //$res = implode("<br />", $tags);
      $res = '';
      foreach($tags as $keyword) {
        $res .= "<p><a href=\"?keyword={$keyword}\"".((trim($keyword) == trim($selected_keyword)) ? " class=\"selected\"" : "").">{$keyword}</a></p>";
      }
      return $res;
    } // _getAllCategories()
    
    // ---------------------------------------------------
    // Check whether my IP address is banned
    // ---------------------------------------------------
    function checkMyIpBanned($ip, $lang = "") {
      $sql = "SELECT * FROM {$this->tbl_prefix}bans WHERE `ip`= '{$ip}' AND now() < date_add(`until`, interval 1 day)";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        //die("I'm sorry, your address is banned.");
        $sql = "SELECT * FROM {$this->tbl_prefix}ban_messages";
        $query = $this->runSQL($sql);
        $row = mysql_fetch_object($query);
        echo $row->{"message".$lang};
        die();
      }
      //$this->addStat();
    } 

    // ---------------------------------------------------
    // BAN IP
    // ---------------------------------------------------
    function banIP($reason = 'No reason specified...') {
    	
    	$banUser = $this->runSQL("INSERT into cc_bans (`ip`, `reason`, `until`) VALUES ('".getIP()."', '".$reason."', date_add(now(),INTERVAL (SELECT ban_period_days FROM `cc_def_settings` WHERE id=1) DAY));");
    	
    }
    
    // ---------------------------------------------------
    // Check whether site is live or not
    // ---------------------------------------------------
    function checkLive($lang = "") {
      $sql = "SELECT * FROM {$this->tbl_prefix}def_settings";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        if ($row->site_offline == 'y') {
        	$lang = ($lang!='') ? "?lang=".substr($lang, 1) : $lang;
		Header("Location: site-under-construction$lang");
		die(); 
	} else {
		return true;
	}
      }
    }

    // ---------------------------------------------------
    // add statistics 
    // ---------------------------------------------------
    function addStat() {
      
      $ip = $_SERVER["REMOTE_ADDR"];
          
      $sql = "
        SELECT TIME_TO_SEC(TIMEDIFF(now(), MAX(`when`))) as time_diff 
        FROM {$this->tbl_prefix}stat
        WHERE `ip`='{$ip}'
      ";
      $query = $this->runSQL($sql);
      $row = mysql_fetch_object($query);

      if ($row->time_diff/60 > 1) {

        $page = $_SERVER["PHP_SELF"];  
      
        if (strstr($_SERVER["HTTP_USER_AGENT"], "Nav")) $browser = "Netscape"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Firefox")) $browser = "Firefox"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "MSIE")) $browser = "MSIE"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Lynx")) $browser = "Lynx"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Opera")) $browser = "Opera"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "WebTV")) $browser = "WebTV"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Konqueror")) $browser = "Konqueror"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Bot")) $browser = "Bot"; 
        else $browser = "Other";
      
        if(strstr($_SERVER["HTTP_USER_AGENT"], "Win")) $os = "Windows"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Mac") || (ereg("PPC", getenv("HTTP_USER_AGENT")))) $os = "Mac"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "Linux")) $os = "Linux"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "FreeBSD")) $os = "FreeBSD"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "SunOS")) $os = "SunOS"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "IRIX")) $os = "IRIX"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "BeOS")) $os = "BeOS"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "OS/2")) $os = "OS/2"; 
        elseif (strstr($_SERVER["HTTP_USER_AGENT"], "AIX")) $os = "AIX"; 
        else $os = "Other"; 
      
        if (isset($_SERVER["HTTP_X_FORWARDED_FOR"])) { 
          $host = gethostbyaddr($_SERVER["HTTP_X_FORWARDED_FOR"]); 
        } else { 
          $host = gethostbyaddr($_SERVER["REMOTE_ADDR"]); 
        }
      
        $referer = (isset($_SERVER["HTTP_REFERER"])) ? $_SERVER["HTTP_REFERER"] : ""; 
      
        $sql = "INSERT INTO {$this->tbl_prefix}stat(`page`, `browser`, `os`, `host`, `ip`, `referer`, `when`) VALUES('{$page}', '{$browser}', '{$os}', '{$host}', '{$ip}', '{$referer}', now())";
        $query = $this->runSQL($sql);

      }
      
    } // addStat()
    
    
    // ---------------------------------------------------
    // get pagination size 
    // ---------------------------------------------------
    function getPagination() {
      $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}def_settings WHERE `id`=1");
      if (mysql_num_rows($query) == 0) {
        $query = $this->runSQL("INSERT INTO {$this->tbl_prefix}def_settings(id) VALUES(1)");
        $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}def_settings WHERE `id`=1");         
      }
      $row = mysql_fetch_object($query);
      return $row->pagination_size;
    } // getPagination()

    function getDefault($param) {
      $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}def_settings WHERE `id`=1");
      if (mysql_num_rows($query) == 0) {
        $query = $this->runSQL("INSERT INTO {$this->tbl_prefix}def_settings(id) VALUES(1)");
        $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}def_settings WHERE `id`=1");         
      }
      $row = mysql_fetch_object($query);
      return $row->$param;
    } // getPagination()


    // ----------------------------------------------
    // get user's permissions
    // ----------------------------------------------
    function getUserPermissions($user_id) {
      // try to get user defined permissions
      $sql = "SELECT * FROM {$this->tbl_prefix}permissions WHERE user_id = '{$user_id}'";      
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {  
        return mysql_fetch_object($query);
      } else {
        // not personal permissions  
        $sql = "SELECT p.* 
          FROM {$this->tbl_prefix}user u 
          JOIN {$this->tbl_prefix}permissions p ON u.status_ID = p.char_status_id AND p.user_id = 0  
          WHERE u.id = '{$user_id}'";
        $query = $this->runSQL($sql);   
        if (mysql_num_rows($query) > 0) {  
          return mysql_fetch_object($query);
        } else {
          $sql = "SELECT p.* 
            FROM {$this->tbl_prefix}user u 
            JOIN {$this->tbl_prefix}permissions p ON p.id  = -1 AND p.user_id = 0  
            WHERE u.id = '{$user_id}'";
          $query = $this->runSQL($sql);   
          return mysql_fetch_object($query);
        }
      }
    } // getUserPermissions($user_id)
    
    
    // ----------------------------------------------
    // get user's permissions
    // ----------------------------------------------
    function getStatusPermissions($user_id) {
      $sql = "SELECT p.* 
        FROM {$this->tbl_prefix}user u  
        JOIN {$this->tbl_prefix}permissions p ON u.status_ID = p.char_status_id AND p.user_id = 0
        WHERE u.id = '{$user_id}'";
      $query = $this->runSQL($sql);
      return mysql_fetch_object($query);
    } // getStatusPermissions($user_id)


    // --------------------------------------------------- 
    // Does this loggined user/admin/superadmin has all the permissions? 
    // ---------------------------------------------------
    function isFullPermitted($user_id) {
      $sql = "
        SELECT define_permissions 
        FROM {$this->tbl_prefix}permissions 
        WHERE user_id='{$user_id}'
      ";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        return $row->define_permissions == 1; 
      } else {
        $sql = "
          SELECT define_permissions 
          FROM {$this->tbl_prefix}user u 
          JOIN {$this->tbl_prefix}permissions p ON p.id = u.permission_id
          WHERE u.id='{$user_id}'
        ";            
        $query = $this->runSQL($sql);
        if (mysql_num_rows($query) > 0) {
          $row = mysql_fetch_object($query);
          return $row->define_permissions == 1; 
        } else {
          return false; 
        }
      }
    } // isFullPermitted($user_id)


    // ---------------------------------------------------
    // only for Admin: 
    //   get permissions (besides ability to define permissions itself)
    // ---------------------------------------------------
    function getPermissionEditableDetails($row, $readonly = false) {
      $ro_b = ($readonly == true) ? "<strong>" : ""; // captions are in <strong> only if read-only
      $ro_b_ = ($readonly == true) ? "</strong>" : ""; // close tag       
      ?>
		<tr>
		  <td><?=$ro_b?>Define Permissions<?=$ro_b_?></td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="define_permissions[]" id="define_permissions_1" value="1" <?php if($row->define_permissions == 1) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <td style="text-align: center;"><input type="radio" name="define_permissions[]" id="define_permissions_0" value="0" <?php if($row->define_permissions == 0) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->define_permissions == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td colspan="4"><br /><?=$ro_b?>Site Content<?=$ro_b_?></td>
		</tr>
		<tr>
		  <td style="text-align: right;">View</td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="site_content_view[]" id="site_content_view_1" value="1" <?php if($row->site_content_view==1) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <td style="text-align: center;"><input type="radio" name="site_content_view[]" id="site_content_view_0" value="0" <?php if($row->site_content_view==0) echo " checked=\"checked\" "; ?> style="width: auto;" onchange="$('#site_content_change_0').attr('checked', true);" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->site_content_view == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td style="text-align: right;">Edit</td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="site_content_change[]" id="site_content_change_1" value="1" <?php if($row->site_content_change==1) echo " checked=\"checked\" "; ?> style="width: auto;" onchange="$('#site_content_view_1').attr('checked', true);" /></td>
      <td style="text-align: center;"><input type="radio" name="site_content_change[]" id="site_content_change_0" value="0" <?php if($row->site_content_change==0) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->site_content_change == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td colspan="4"><br /><?=$ro_b?>User Details<?=$ro_b_?></td>
		</tr>
		<tr>
		  <td style="text-align: right;">View</td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="user_details_view[]" id="user_details_view_1" value="1" <?php if($row->user_details_view==1) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <td style="text-align: center;"><input type="radio" name="user_details_view[]" id="user_details_view_0" value="0" <?php if($row->user_details_view==0) echo " checked=\"checked\" "; ?> style="width: auto;" onchange="$('#user_details_change_0').attr('checked', true);" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->user_details_view == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td style="text-align: right;">Edit</td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="user_details_change[]" id="user_details_change_1" value="1" <?php if($row->user_details_change==1) echo " checked=\"checked\" "; ?> style="width: auto;" onchange="$('#user_details_view_1').attr('checked', true);" /></td>
      <td style="text-align: center;"><input type="radio" name="user_details_change[]" id="user_details_change_0" value="0" <?php if($row->user_details_change==0) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->user_details_change == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		<tr><td colspan="5">&nbsp;</td></tr>
		<tr>
		  <td><?=$ro_b?>Edit Language<?=$ro_b_?></td>
		  <?php if (!$readonly) { ?>
		  <td style="text-align: center;"><input type="radio" name="edit_languages[]" id="edit_languages_1" value="1" <?php if($row->edit_languages == 1) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <td style="text-align: center;"><input type="radio" name="edit_languages[]" id="edit_languages_0" value="0" <?php if($row->edit_languages == 0) echo " checked=\"checked\" "; ?> style="width: auto;" /></td>
      <?php } else { echo "<td style=\"text-align: center;\">".(($row->edit_languages == 1) ? "Allowed" : "Denied")."</td>"; } ?>
      <td>&nbsp;</td>
		</tr>
		
      <?php    
    } // getPermissionEditableDetails($row)


    // ---------------------------------------------------
    // only for Admin: 
    //   get permissions (besides ability to define permissions itself)
    // ---------------------------------------------------
    function getPermissionViewDetails($row) {
      ?>
		<tr>
		  <td colspan="4"><br />Site Content</td>
		</tr>
		<tr>
		  <td style="text-align: right;">View</td>
		  <td style="text-align: center;"><input type="radio" name="site_content_view[]" id="site_content_view_1" value="1" <?php if($row->site_content_view==1) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td style="text-align: center;"><input type="radio" name="site_content_view[]" id="site_content_view_0" value="0" <?php if($row->site_content_view==0) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td style="text-align: right;">Edit</td>
		  <td style="text-align: center;"><input type="radio" name="site_content_change[]" id="site_content_change_1" value="1" <?php if($row->site_content_change==1) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled"  /></td>
      <td style="text-align: center;"><input type="radio" name="site_content_change[]" id="site_content_change_0" value="0" <?php if($row->site_content_change==0) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td colspan="4"><br />User Details</td>
		</tr>
		<tr>
		  <td style="text-align: right;">View</td>
		  <td style="text-align: center;"><input type="radio" name="user_details_view[]" id="user_details_view_1" value="1" <?php if($row->user_details_view==1) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td style="text-align: center;"><input type="radio" name="user_details_view[]" id="user_details_view_0" value="0" <?php if($row->user_details_view==0) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled"  /></td>
      <td>&nbsp;</td>
		</tr>
		<tr>
		  <td style="text-align: right;">Edit</td>
		  <td style="text-align: center;"><input type="radio" name="user_details_change[]" id="user_details_change_1" value="1" <?php if($row->user_details_change==1) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td style="text-align: center;"><input type="radio" name="user_details_change[]" id="user_details_change_0" value="0" <?php if($row->user_details_change==0) echo " checked=\"checked\" "; ?> style="width: auto;" disabled="disabled" /></td>
      <td>&nbsp;</td>
		</tr>
      <?php    
    } // getPermissionEditableDetails($row)


    // ---------------------------------------------------
    // Check access when you are trying to access not permitted page 
    // ---------------------------------------------------
    function checkAccess() {
      global $_SESSION;
      global $can_define_permissions;
      
      if (!isset($_SESSION["user"]["is_admin"]) || !$can_define_permissions) { 
        echo '<script type="text/javascript">history.back();</script>'; die(); 
      }
    } // checkAccess()
    
    
    // ---------------------------------------------------
    // Get tribes list to the front-end 
    // ---------------------------------------------------
    // $lang = <empty>, _ae etc.
    // ---------------------------------------------------
    function getTribes($lang) {
      global $data;
      $data['tribes'] = array();
      // get total population
      $sql = "SELECT SUM(population) as total_population FROM {$this->tbl_prefix}tribes WHERE active=1";      
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $total = $row->total_population;
      } else $total = 0;      
      // which tribe has won the most battles?
      $sql = "SELECT t.ID as tribe_id, u.username, t.population, t.battles_won
        FROM {$this->tbl_prefix}tribes t
        LEFT JOIN {$this->tbl_prefix}user u ON t.chief_id = u.ID
        WHERE t.active=1  
        ORDER BY 1";              
      $query = $this->runSQL($sql);      
      $tribes = $tribe_rank_ids = $tribe_ranks = $tribe_chiefs = $tribe_populations = array();      
      while ($row = mysql_fetch_object($query)) {
        $id = sprintf("%02d", $row->tribe_id);
        $percent_pop = ($total > 0) ? $row->population / $total : 0;
        $percent_pop = round($percent_pop*100);
        array_push($tribes, $id);
        array_push($tribe_chiefs, $row->username);      
        $battles_won = explode(";", $row->battles_won);
        $battles_won_perc = (isset($battles_won[1]) && ($battles_won[1] > 0)) ? $battles_won[0] / $battles_won[1] : 0; 
        array_push($tribe_rank_ids, $row->tribe_id);  
        array_push($tribe_ranks, $battles_won_perc);
        array_push($tribe_populations, $percent_pop);
      }      
      // sort tribe ranks (ONLY, do not touch other params)
      for($i=0; $i<count($tribe_rank_ids); $i++) {
        for($j=count($tribe_rank_ids)-1; $j>$i; $j--) {
          if ($tribe_ranks[$i] < $tribe_ranks[$j]) {
            $k = $tribe_ranks[$i]; 
            $tribe_ranks[$i] = $tribe_ranks[$j];
            $tribe_ranks[$j] = $k;
          }
        }
      }      
      // write the places in words            
      $counts = array(
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_1'", "1st"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_2'", "2st"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_3'", "3rd"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_4'", "4th"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_5'", "5th"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_6'", "6th"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_7'", "7th"),
        $this->getTableValue("friendly_name".$lang, "translations_system", "name='place_8'", "8th")
      );        
      for($i=0; $i<count($tribe_rank_ids); $i++) {   
        $tribe_ranks[$tribe_rank_ids[$i]-1] = $counts[$i];
      }      
      // output the tribes
      for($i=0; $i<count($tribes); $i++) {
        $ar["id"] = $tribes[$i];
        $ar["rank"] = $tribe_ranks[$i];
        $ar["chief"] = $tribe_chiefs[$i];
        $ar["population"] = $tribe_populations[$i];
        array_push($data['tribes'], $ar); 
      }
      
    } // getTribes()


    function _getTribes() {
      // get total population
      $sql = "SELECT SUM(population) as total_population FROM {$this->tbl_prefix}tribes WHERE active=1";
//echo $sql.'<br /><br />';      
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);
        $total = $row->total_population;
      } else $total = 0;
      
      // which tribe has won the most battles?
      //$sql = "SELECT * FROM {$this->tbl_prefix}tribes ";
    
      $sql = "SELECT t.ID as tribe_id, u.username, t.population, t.battles_won
        FROM {$this->tbl_prefix}tribes t
        LEFT JOIN {$this->tbl_prefix}user u ON t.chief_id = u.ID
        WHERE t.active=1  
        ORDER BY 1";              
      $query = $this->runSQL($sql);
//echo $sql;      
      $tribes = $tribe_rank_ids = $tribe_ranks = $tribe_chiefs = $tribe_populations = array();
      
      while ($row = mysql_fetch_object($query)) {
        $id = sprintf("%02d", $row->tribe_id);
        $percent_pop = ($total > 0) ? $row->population / $total : 0;
        $percent_pop = round($percent_pop*100);
        array_push($tribes, $id);
        array_push($tribe_chiefs, $row->username);      
        $battles_won = explode(";", $row->battles_won);
        $battles_won_perc = (isset($battles_won[1]) && ($battles_won[1] > 0)) ? $battles_won[0] / $battles_won[1] : 0; 
        array_push($tribe_rank_ids, $row->tribe_id);  
        array_push($tribe_ranks, $battles_won_perc);
        array_push($tribe_populations, $percent_pop);
      }
      
      // sort tribe ranks (ONLY, do not touch other params)
      for($i=0; $i<count($tribe_rank_ids); $i++) {
        for($j=count($tribe_rank_ids)-1; $j>$i; $j--) {
          if ($tribe_ranks[$i] < $tribe_ranks[$j]) {
            
            $k = $tribe_ranks[$i]; 
            $tribe_ranks[$i] = $tribe_ranks[$j];
            $tribe_ranks[$j] = $k;
  
            /*$k = $tribe_rank_ids[$i]; 
            $tribe_rank_ids[$i] = $tribe_rank_ids[$j];
            $tribe_rank_ids[$j] = $k;
            
            $k = $tribe_populations[$i];
            $tribe_populations[$i] = $tribe_populations[$j];
            $tribe_populations[$j] = $k;*/
  
          }
        }
      }
      
      // write the places in words
      $eng_counts = array("1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th");
      for($i=0; $i<count($tribe_rank_ids); $i++) {   
        $tribe_ranks[$tribe_rank_ids[$i]-1] = $eng_counts[$i];
      }
      
      // output the tribes
      for($i=0; $i<count($tribes); $i++) {
        echo "
        <div class=\"tribe_column tribe_{$tribes[$i]}\">
          <div class=\"details\">{$tribe_ranks[$i]}<p>{$tribe_chiefs[$i]}&nbsp;</p><p style=\"margin-top: 3px; margin-left: 42px;\">{$tribe_populations[$i]} %</p></div>
        </div>
        ";
      }
      
    } // _getTribes()
    
    

    // ---------------------------------------------------
    // Update user's house 
    // If the player already has a home, and we change their status to the lower level 
    // defined as min.level char_status.is_min_level_home 
    // it's 'picking up' all the furniture from the home and returning it to the player, and then removing this home.
    // ---------------------------------------------------
    function updateUserHouse($user_id, $new_status_id, $remove_anyway = false) {
    
      if ($user_id == 0) return "";
      
      $sql = "SELECT * FROM {$this->tbl_prefix}user WHERE ID = '{$user_id}'";       
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) { 
        $row = mysql_fetch_object($query);
        $status_ID = $row->status_ID;
        $tribe_ID = $row->tribe_ID;
        
        if ($remove_anyway == true)
          // status_ID == 7 -- Moderator
          // status_ID == 8 -- Administrator
          $remove_anyway = ($status_ID != 7 AND $status_ID != 8); // for admin and mods it's not possible to delete homes

        // get ID of minimum level of creating the home
        $sql_status = "SELECT * FROM {$this->tbl_prefix}char_status WHERE is_min_level_home='1'";       
        $query_status = $this->runSQL($sql_status);
        $row_status = mysql_fetch_object($query_status);

        // we are increasing level --> need to create home?          
        if (($status_ID <= $row_status->ID) && ($new_status_id > $row_status->ID)) {                         
          // Check they don't have a home already
          $sql = "SELECT count(ID) as counter FROM {$this->tbl_prefix}homes WHERE user_id = '{$user_id}'";                  
          $query = $this->runSQL($sql);
          if (mysql_num_rows($query) > 0) {
            $row = mysql_fetch_object($query);
            if ($row->counter == 0) {
              $home_rental_period_days = $this->getDefault('home_rental_period_days');
              // Give them a home
              $sql = "SELECT `ID` FROM {$this->tbl_prefix}homes WHERE user_id = 0 AND tribe_ID = {$tribe_ID} ORDER BY `ID`";               
              $query = $this->runSQL($sql);              
              if (mysql_num_rows($query) > 0) { // founded empty home
                $row = mysql_fetch_object($query);
                // settle into this house
                $sql = "UPDATE {$this->tbl_prefix}homes SET user_id = '{$user_id}', `door_state`=0, `expiry_date` = DATE_ADD(NOW(), INTERVAL {$home_rental_period_days} DAY) WHERE ID={$row->ID}";                                        
                $query = $this->runSQL($sql);
                return "Home created successfully.";  
              } else {
                // What is the max home number ?
                $sql = "SELECT MAX(street_num) as `max_street` FROM {$this->tbl_prefix}homes WHERE tribe_ID = '{$tribe_ID}'";                     
                $query = $this->runSQL($sql);
                if (mysql_num_rows($query) > 0) {
                  $row = mysql_fetch_object($query);
                  $maxstreet = $row->max_street + 1;
                  // create home on the next street
                  $sql = "INSERT INTO {$this->tbl_prefix}homes (`ID`,`street_num`,`user_id`,`tribe_ID`,`door_state`,`created`, `expiry_date`) VALUES ('NULL','$maxstreet','$user_id','$tribe_ID','0', NOW(), DATE_ADD(NOW(), INTERVAL {$home_rental_period_days} DAY) )";                       
                  $query = $this->runSQL($sql);
                  return "Home created successfully."; 
                }
              }
            }
          } //else return "This user already probably has a home!";                    
        } //else return "This user is already upgraded.";
        
        // Remove anyway OR we are decreasing level --> get out all the furniture  
        if ($remove_anyway == true || $new_status_id < $row_status->ID) {
        
          // pick up all the furniture
          $sql = "SELECT * FROM {$this->tbl_prefix}inv WHERE active=0 AND user_id = '{$user_id}'";    
          $query = $this->runSQL($sql);
          $ids = array();
          if (mysql_num_rows($query) > 0) {
            while ($row = mysql_fetch_object($query)) {
              // get IDs of the furniture
              array_push($ids, $row->ID);
            }
          }
          
          $return_status = "";
          // if any furniture exists...
          if (count($ids) > 0) {
            // get all the furniture from the home to the personal equipment 
            $sql = "UPDATE {$this->tbl_prefix}inv SET active=1 WHERE ID IN (" . implode(",", $ids ) .")";     
            $query = $this->runSQL($sql);
            // delete records from homes_furniture
            $sql = "DELETE FROM {$this->tbl_prefix}homes_furniture WHERE inv_id IN (" . implode(",", $ids ) .")";     
            $query = $this->runSQL($sql);
            $return_status .= "All the furniture was picked up.<br />";
          }
          
          // status_ID == 7 -- Moderator
          // status_ID == 8 -- Administrator          
          if ($status_ID != 7 || $new_status_id != 4) { // delete except if Moderator -> Explorer  
          //if ($status_ID != 7 AND $status_ID != 8) { // delete house only for NOT mods/admins
            // exists home for this user?
            $sql = "SELECT * FROM {$this->tbl_prefix}homes WHERE user_id = '{$user_id}'"; 
            $query = $this->runSQL($sql);
            if (mysql_num_rows($query) > 0) {
              // remove the home
              $sql = "UPDATE {$this->tbl_prefix}homes SET user_id=0, expiry_date = NULL WHERE user_id='{$user_id}'";     
              $query = $this->runSQL($sql);
              $return_status .= "Home was removed successfully.";
            }
          }
          
          return $return_status;
          
        } // decreasing level
        
      }
    } // updateUserHouse($user_id)
    

    // ---------------------------------------------------
    // Get Translation
    // $lang = "", "_ae", etc.  
    // ---------------------------------------------------
    function getTranslation($value, $lang = "", $table = "translations", $field = "caption", $result_field = "name", $debug = false) {
      
      $sql = "SELECT * FROM {$this->tbl_prefix}{$table} WHERE {$field} = '{$value}'";                  
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
       $row = mysql_fetch_object($query);
       return htmlspecialchars_decode($row->{$result_field.$lang});
      } else return "";
            
    } // getTranslation($caption, $lang)
    

    // ---------------------------------------------------
    // Get the drop-down list of all extra languages 
    // ---------------------------------------------------
    function getLanguagesCbx($lang = "", $css = "") {
      $css = ($css != "") ? " style=\"{$css}\"" : ""; 
      $res  = "<select name=\"lang_id\" id=\"lang_id\" {$css}>";
      $res .= "<option value=\"0\""; if ($lang == "" || $lang == "_gb") $res .= " selected=\"selected\""; $res .= ">English</option>";
      $sql = "SELECT * FROM {$this->tbl_prefix}extra_langs ORDER BY `lang`";
      $query = $this->runSQL($sql);
      while ($row = mysql_fetch_object($query)) {
        $res .= "<option value=\"{$row->id}\"";
        if ($lang == $row->lang) $res .= " selected=\"selected\"";
        $res .= ">{$row->name}</option>";
      }      
      $res .= "</select>";
      return $res;
    } // getLanguagesCbx($lang, $css)

    
    // ---------------------------------------------------
    // Get the drop-down list of all available extra countries (that is not used yet) 
    // ---------------------------------------------------
    function getFreeLanguagesCbx($lang = "", $css = "") {
      $css = ($css != "") ? " style=\"{$css}\"" : ""; 
      $res  = "<select name=\"lang_id\" id=\"lang_id\" {$css}><option value=\"0\" selected=\"selected\">&nbsp;</option>";
      
      $sql = "
        SELECT * 
        FROM {$this->tbl_prefix}countries
        WHERE id NOT IN (77, 14, 226, 228) AND
          id NOT IN (SELECT country_id FROM {$this->tbl_prefix}extra_langs) 
        ORDER BY `code`";

      $query = $this->runSQL($sql);
      while ($row = mysql_fetch_object($query)) {
        $res .= "<option value=\"{$row->id}\">{$row->name} [{$row->code}]</option>";
      }      
      $res .= "</select>";
      return $res;
    } // getFreeLanguagesCbx($lang, $css)
    
    // ---------------------------------------------------
    // Get the ID of language what is using on the front-end 
    // ---------------------------------------------------
    function getCurLangId($lang = "") {
      if ($lang == "") return 0;
      $sql = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE lang='".substr($lang, 1)."'";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);            
        return $row->id;
      } else return 0;
    } // getCurLangId($lang)


    // ---------------------------------------------------
    //  
    // ---------------------------------------------------
    function getCurLang($lang = "") {
      if ($lang == "") return "";
      $sql = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE lang='".substr($lang, 1)."'";      
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        $row = mysql_fetch_object($query);            
        return "_".$row->lang;
      } else return "";
    } // getCurLangId($lang)  
  
  
    // ---------------------------------------------------
    // Get the ID of language what is using on the front-end 
    // ---------------------------------------------------
    function showLanguagesPanel($cur_lang = "") { ?>
      <div id="langs"><?php
        if (count($this->langs_available) > 1) {
          for($i = 1; $i < count($this->langs_available); $i++) {
            $alt = strtoupper($this->langs_available[$i]);
            $img_url = "images/flags/site/".$this->langs_available[$i].(($cur_lang == "_".$this->langs_available[$i]) ? "_active" : "").".png";        
            if (!file_exists($img_url)) $img_url = "images/flags/{$this->langs_available[$i]}.png";
            echo "<a href=\"{$this->url_root}/{$this->langs_available[$i]}/\"><img src=\"{$this->url_root}/{$img_url}\" alt=\"{$alt}\" title=\"{$alt}\" /></a>";
          }
          $img_url = "images/flags/site/".$this->langs_available[0].(($cur_lang == "") ? "_active" : "").".png";        
          echo "<a href=\"{$this->url_root}/\"><img src=\"{$this->url_root}/{$img_url}\" alt=\"".strtoupper($this->langs_available[0])."\" /></a>";
        } ?>  
      </div><?php

        
    } // showLanguagesPanel($cur_lang)  


    function getLanguagesPanel($cur_lang = "") {
      global $data;          
      if (count($this->langs_available) > 1) {
        for($i = 1; $i < count($this->langs_available); $i++) {
          $img_url = $this->langs_available[$i].(($cur_lang == "_".$this->langs_available[$i]) ? "_active" : "").".png";
          $ar["image"] = $img_url;        
          $img_url = "images/flags/site/".$img_url; 
          if (!file_exists($img_url)) $img_url = "images/flags/{$this->langs_available[$i]}.png";
          $ar["href"] = "{$this->url_root}/{$this->langs_available[$i]}/";
          $ar["alt"] = strtoupper($this->langs_available[$i]);
          $ar["flag_image"] = "{$this->url_root}/{$img_url}";
          array_push($data['available_languages'], $ar);
        } 
        $img_url = $this->langs_available[0].(($cur_lang == "") ? "_active" : "").".png";
        $ar["image"] = $img_url;       
        $img_url = "images/flags/site/".$img_url;
        $ar["href"] = "{$this->url_root}/";
        $ar["alt"] = strtoupper($this->langs_available[0]);
        $ar["flag_image"] = "{$this->url_root}/{$img_url}";
        array_push($data['available_languages'], $ar);
      } 
    } 


    function getAdminsEmail() {      
      $emails = array();
      /*$sql = "SELECT u.first_name, u.last_name, u.email, el.lang 
        FROM `{$this->tbl_prefix}user` u
        LEFT JOIN `{$this->tbl_prefix}extra_langs` el ON u.lang_id = el.id 
        WHERE u.status_ID=7 OR u.status_ID=8";
      $query = $this->runSQL($sql);
      if (mysql_num_rows($query) > 0) {
        while($row = mysql_fetch_object($query)) {
          $ar["first_name"] = $row->first_name;
          $ar["last_name"] = $row->last_name;
          $ar["email"] = $row->email;
          $ar["lang"] = $row->lang;
          $ar["lang_"] = ($row->lang != '') ? "_".$row->lang : '';
          array_push($emails, $ar);
        }
      }*/
          $ar["first_name"] = "sergey";
          $ar["last_name"] = "markov";
          $ar["email"] = "sergey.markov@gmail.com";
          $ar["lang"] = "ru";
          $ar["lang_"] = "_ru";
          array_push($emails, $ar);
                 
      return $emails;
    }

    // ---------------------------------------------------
    // Get the ID of language what is using on the front-end 
    // ---------------------------------------------------
    function parseURI(&$URIelements, &$render, &$sect, &$page, &$lang) {
                      
      // get from the environment variable with root_path, if it's empty - we are in the root 
      $URI = $_SERVER['REQUEST_URI'];
      
      // parse URL   
      $URIelements = array_values(array_filter(explode('/', $URI)));
      
      $lang = "";
      // was selected other language
      if (count($URIelements) > 0 && strlen($URIelements[0]) == 2) {        
        if (!in_array($URIelements[0], $this->langs)) 
          $lang = ""; else 
          $lang = "_".$URIelements[0];
        array_shift($URIelements);
      }
 
      $render = "index.php";
      $sect = 0;
      $page = 117;

      if (count($URIelements) > 0) {
        $params_pos = strpos($URIelements[0], "?");
        if ($params_pos > 0) $URIelements[0] = substr($URIelements[0], 0, $params_pos);
        $sql = "SELECT * FROM {$this->tbl_prefix}url_dispatch WHERE url=\"{$URIelements[0]}\""; //echo $sql;        
        $query = $this->runSQL($sql);
        if (mysql_num_rows($query) > 0) {
          $row = mysql_fetch_object($query);            
          $render = $row->render;
          $sect = $row->sect;
          $page = $row->page;
        }
      }
    } // parseURI()


    
    function array_push_assoc($array, $key, $value) {
      $array[$key] = $value;
      return $array;
    }

    // ---------------------------------------------------
    //
    // ---------------------------------------------------
    function getFields($table_name, &$fields) {
      $query = $this->runSQL("SHOW FIELDS FROM {$this->tbl_prefix}{$table_name}");
      while($row = mysql_fetch_object($query)) {
        if (substr($row->Type, 0, 7)=="varchar") {
        $s = substr($row->Type, 8);
        $s = substr($s, 0, -1);
        $fields = $this->array_push_assoc($fields, $row->Field, $s);
        }
      }
    }


    function getLabels($lang, &$labels) {    
      $query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}translations_system");
      //$query = $this->runSQL("SELECT * FROM {$this->tbl_prefix}translations_label");
      while($row = mysql_fetch_object($query)) {
        $labels = $this->array_push_assoc($labels, $row->name, $row->{"friendly_name".$lang});
      }
    }

    /*function getFieldLength($field_name) {
      return $fields[$field];
    }*/


    // ---------------------------------------------------
    // Show input control
    // $id - string 
    // ---------------------------------------------------
    function input($id, $type, $is_mandatory = false, $readonly = false, $css = '') {
      global $fields, $friendly_names, $row, $cur_lang_;
      
      if ($type != "hidden") {
        if (array_key_exists($id, $friendly_names)) {
          $friendly_name =  $friendly_names[$id];
        } else {
          $friendly_name =  ucwords(str_replace("_", " ", $id));
        }
        $friendly_name = ($friendly_name != '') ? $friendly_name.':' : "&nbsp;";
      }
      $max_length = (array_key_exists($id, $fields)) ? $fields[$id] : 15;
      $width = $max_length*6;

      $value = (isset($row) && isset($row->$id)) ? html_entity_decode(htmlspecialchars(stripslashes($row->$id))) : "";
      
      if ($type != "hidden") echo "<div class=\"table_row\">";
      switch ($type) {
        case "label":
        case "label_YN":
        case "label_select":
          echo "<label for=\"{$id}\">{$friendly_name}</label>";
          if ($type == "label_YN") $value = ($value==0) ? "Yes" : "No";
          if ($type == "label_select") {
            //$sql1 = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE id='{$value}'";
          /*
              $sql1 = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE id='{$value}'";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) == 0) $lang = ""; else {
                $row1 = mysql_fetch_object($query1);
                $lang = $row1->lang;
              } 
              echo "<option value=\"0\"".(($lang == "") ? " selected=\"selected\"" : "").">English</option>";
              $sql1 = "SELECT * FROM {$this->tbl_prefix}extra_langs ORDER BY `lang`";
              $query1 = $this->runSQL($sql1);
              while ($row1 = mysql_fetch_object($query1)) {
                echo "<option value=\"{$row1->id}\"".(($lang == $row1->lang) ? " selected=\"selected\"" : "").">{$row1->name}</option>";
              }
          */
          }
          echo "<div style=\"padding-top: 4px;\">".strip_tags(html_entity_decode($value))."</div>";
          break;        
        case "fid":
			if(isset($value)) {
				echo "<label for=\"{$id}\">Facebook:</label>";
				?>
				<a href="https://facebook.com/<?php echo $value;?>" target="_blank">
				<img src="https://graph.facebook.com/<?php echo $value;?>/picture">&nbsp;
				<?php echo $value;?>
				</a>
				<?php
			}
		break;
        case "text":
		
        case "date":
        case "datetime":
          if ($width > 250) $width = 250;
          if ($id=="register_date" || $id=="sent_when") $width = 200;
          $css_style = "width: {$width}px;";
          $css_style .= $css;
          if ($readonly) $css_style .= " background-color: #FAFAFA;"; 
          echo "<label for=\"{$id}\">{$friendly_name}".(($is_mandatory) ? "<span style=\"color: #cc0000;\">*</span>" : "")."</label>";
          echo "<input type=\"text\" id=\"{$id}\" name=\"{$id}\" value=\"{$value}\" maxlength=\"{$max_length}\" style=\"{$css_style}\" "
            .(($is_mandatory) ? "class=\"required\" " : "")
            .(($readonly) ? "readonly=\"readonly\"" : "")
            ." />"
            .(($is_mandatory) ? "<span id=\"span_{$id}_required\" class=\"warning_required\"></span>" : "")
            ;  
          break; // text/date
          
        case "hidden":
          echo "<p><input type=\"hidden\" id=\"{$id}\" name=\"{$id}\" value=\"{$value}\" /></p>";
          break; // hidden
          
        case "select":
          echo "<label for=\"{$id}\">{$friendly_name}".(($is_mandatory) ? "<span style=\"color: #cc0000;\">*</span>" : "")."</label>";
          echo "<select id=\"{$id}\" name=\"{$id}\"".(($is_mandatory) ? "class=\"required\" " : "").">";
          switch ($id) {
            case "country_id":
              $sql1 = "
                  SELECT * 
                  FROM {$this->tbl_prefix}countries
                  WHERE active=1
                  ORDER BY `name`
                  ";
              $query1 = $this->runSQL($sql1); 
              echo "<option value=\"-1\" ".(($value == 0) ? " selected=\"selected\"" : "").">&nbsp;</option>";
              while ($row1 = mysql_fetch_object($query1)) {
                $selected = ($row1->id == $value) ? " selected=\"selected\"" : ""; 
                echo "<option value=\"{$row1->id}\" {$selected}>{$row1->name}</option>"; 
              }
              break; // select / country_id
            case "status_ID":
              $sql1 = "SELECT * FROM {$this->tbl_prefix}char_status WHERE display=1 AND ID='{$value}'";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) == 0 || $value=="") echo "<option value=\"-1\" selected=\"selected\">&nbsp;</option>";
              $query1 = $this->runSQL("SELECT * FROM {$this->tbl_prefix}char_status WHERE display=1 ORDER BY `ID`");
              while ($row1 = mysql_fetch_object($query1)) {
                echo "<option value=\"{$row1->ID}\"";
                if ($row1->ID == $value) echo " selected=\"selected\"";
                echo ">".$row1->friendly_name;
                echo "</option>";
              }
              break; // select / status_ID
            case "lang_id":
              // get default language friendly name
              $sql1 = "SELECT * FROM {$this->tbl_prefix}def_lang";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) == 0) $def_lang = "English"; else {
                $row1 = mysql_fetch_object($query1);
                $def_lang = $row1->friendly_name;
              }
              $sql1 = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE id='{$value}' AND active=1";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) == 0) $lang = ""; else {
                $row1 = mysql_fetch_object($query1);
                $lang = $row1->lang;
              } 
              echo "<option value=\"0\"".(($lang == "") ? " selected=\"selected\"" : "").">{$def_lang}</option>";
              $sql1 = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE active=1 ORDER BY `lang`";
              $query1 = $this->runSQL($sql1);
              while ($row1 = mysql_fetch_object($query1)) {
                echo "<option value=\"{$row1->id}\"".(($lang == $row1->lang) ? " selected=\"selected\"" : "").">{$row1->name}</option>";
              }
              break; // select / lang_id
            case "server_id":
              $sql1 = "SELECT id, `name`, `ip` FROM {$this->tbl_prefix}servers ORDER BY `ID`";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) > 0) {
                while ($row1 = mysql_fetch_object($query1)) { 
                  echo "<option value=\"{$row1->id}\">{$row1->name} ({$row1->ip})</option>";
                }
              }
              break; // select / server_id
            case "tribe_ID":
              $sql1 = "SELECT ID, `name`, currency FROM {$this->tbl_prefix}tribes ORDER BY `ID`";
              if ($value=="") echo "<option value=\"-1\" selected=\"selected\">&nbsp;</option>";
              $query1 = $this->runSQL($sql1);
              if (mysql_num_rows($query1) > 0) {
                $tribe_id = 1;
                while ($row1 = mysql_fetch_object($query1)) { 
                  echo "<option value=\"{$tribe_id}\">{$row1->name} ({$row1->currency})</option>";
                  ++$tribe_id;
                }
              }
              break; // select / tribe_ID  
            case "zone_status":
              $values = array("", "RESTART", "RESTARTING");
              foreach ($values as $cur_value) {
                echo "<option "; 
                if ($value == $cur_value) echo " selected=\"selected\""; 
                echo ">{$cur_value}</option>";
              }
              break; // select / zone_status  
          }
          echo "</select>".(($is_mandatory) ? "<span id=\"span_{$id}_required\" class=\"warning_required\"></span>" : "");
          break; // select
          
          case "radio_YN":
            echo "<label for=\"{$id}_Y\">{$friendly_name}".(($is_mandatory) ? "<span style=\"color: #cc0000;\">*</span>" : "")."</label>";
            echo "<input type=\"radio\" name=\"{$id}\" id=\"{$id}_Y\" ".(($value == '1' || $value == "") ? "checked=\"checked\"" : "")." value=\"1\" /> Yes";
            echo "<input type=\"radio\" name=\"{$id}\" id=\"{$id}_N\" ".(($value == '0') ? "checked=\"checked\"" : "")." value=\"0\" /> No";
            break; // radio_YN
          
          case "radio_MF":
            echo "<label for=\"{$id}_M\">{$friendly_name}".(($is_mandatory) ? "<span style=\"color: #cc0000;\">*</span>" : "")."</label>";
            echo "<input type=\"radio\" name=\"{$id}\" id=\"{$id}_M\" ".(($value == 'M') ? "checked=\"checked\"" : "")." value=\"M\" ".(($is_mandatory) ? "class=\"required\" " : "")." /> Male";
            echo "<input type=\"radio\" name=\"{$id}\" id=\"{$id}_F\" ".(($value == 'F') ? "checked=\"checked\"" : "")." value=\"F\" ".(($is_mandatory) ? "class=\"required\" " : "")." /> Female";
            if ($is_mandatory) echo "<span id=\"span_{$id}_required\" class=\"warning_required\"></span>";
            break; // radio_MF
        
        case "textarea":
          echo "<label for=\"{$id}_M\">{$friendly_name}".(($is_mandatory) ? "<span style=\"color: #cc0000;\">*</span>" : "")."</label>";
          echo "<textarea name=\"{$id}\" id=\"{$id}\" style=\"width: 250px;\">{$value}</textarea>";
          break;          
      }
      if ($type != "hidden") echo "</div>";
    } // input


    function log_user_action($subject, $subject_id, $action, $deleted_value = '', $subject_field = '', $old_value = '') {
      
      $subject = htmlspecialchars($subject, ENT_QUOTES);
      $subject_id = (int)$subject_id;
      $action = htmlspecialchars($action, ENT_QUOTES);
      $deleted_value = htmlspecialchars($deleted_value, ENT_QUOTES);
      $subject_field = htmlspecialchars($subject_field, ENT_QUOTES);
      $old_value = htmlspecialchars($old_value, ENT_QUOTES);
      
      $sql = "INSERT INTO {$this->tbl_prefix}user_actions_history(`user_id`, `subject`, `subject_id`, `action`, `when`, `deleted_value`, `subject_field`, `old_value`) VALUES('".$_SESSION["user"]["id"]."', '{$subject}', '{$subject_id}', '{$action}', NOW(), '{$deleted_value}', '{$subject_field}', '{$old_value}')";
      $query = $this->runSQL($sql);      
    } // log_user_action
    
    
    
    function getTextDirection($lang) {
      // get default text direction 
      if ($lang == "") {
        return $this->default_text_direction;
      } else {
		$lang = addslashes($lang);
        $sql = "SELECT * FROM {$this->tbl_prefix}extra_langs WHERE lang='".substr($lang, 1)."'";
        $query = $this->runSQL($sql);
        $row = mysql_fetch_object($query);
        return $row->direction;
      }
    } // getTextDirection

    
    function getEmailFrom() {
      $sql = "SELECT * FROM {$this->tbl_prefix}def_settings";
      $query = $this->runSQL($sql);
      $row = mysql_fetch_object($query);
      return $row->email_from;
    }
          
  
    function getInitPurseAmount() {
      $sql = "SELECT * FROM {$this->tbl_prefix}def_settings";
      $query = $this->runSQL($sql);
      $row = mysql_fetch_object($query);
      return $row->init_purse_amount;
    }
    
    
    
    function dropDbField($table, $field_name, $lang_code) {
       $sql = "SHOW FIELDS FROM `{$this->tbl_prefix}{$table}` WHERE Field='{$field_name}_{$lang_code}'";
       $query = $this->runSQL($sql);
       if (mysql_num_rows($query) > 0) {       
         $sql = "ALTER TABLE `{$this->tbl_prefix}{$table}` DROP `{$field_name}_{$lang_code}`"; 
         $query = $this->runSQL($sql);
       }
    }
    function addDbField($table, $field_name, $lang_code, $field_type) {
       $sql = "SHOW FIELDS FROM `{$this->tbl_prefix}{$table}` WHERE Field='{$field_name}_{$lang_code}'";
       $query = $this->runSQL($sql);
       if (mysql_num_rows($query) == 0) {
         $sql = "ALTER TABLE `{$this->tbl_prefix}{$table}` ADD `{$field_name}_{$lang_code}` {$field_type}";          
         $query = $this->runSQL($sql);
       }       
    }
    
    
    // ----------------------------------------
    // Write into error log
    // in the format <current_datetime> <URL with error/warning> <error_type> <message>
    // ----------------------------------------
    // Params:
    // $msg: <php-file-name> error message   
    // $error_type: 0(def) - warning, 1 - error, 2 - critical error
    // ----------------------------------------
    function write_log($msg, $error_type = 0) {
      $handle = fopen("cocolani.log", 'a');
      switch ($error_type) {
        case 1: $error_type = "ERROR: "; break; 
        case 2: $error_type = "CRITICAL ERROR: "; break;
        default:
      }      
      fwrite($handle, date("r")." ".$_SERVER["REQUEST_URI"]." ".$error_type." ".$msg."\r\n");
      fclose($handle);
    }
  
  
  } // end of class




function getIP() { 
	$ip; 
	if (getenv("HTTP_CLIENT_IP")) $ip = getenv("HTTP_CLIENT_IP"); 
		else if(getenv("HTTP_X_FORWARDED_FOR")) $ip = getenv("HTTP_X_FORWARDED_FOR"); 
			else if(getenv("REMOTE_ADDR")) $ip = getenv("REMOTE_ADDR"); 
				else $ip = "UNKNOWN";
	return $ip; 
}


