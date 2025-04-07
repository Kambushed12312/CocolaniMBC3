<?php
	include_once("includes/db.php");
	include_once("settings.php");
	$db = new database($obj->get("db_name"), $obj->get("db_server"), $obj->get("db_user"), $obj->get("db_password"), $obj->get("url_root"));

    $username = isset($_POST['username']) ? mysqli_real_escape_string($_POST['username']) : "";
	$password = isset($_POST['password']) ? mysqli_real_escape_string($_POST['password']) : "";
	$email = isset($_POST['email']) ? mysqli_real_escape_string($_POST['email']) : '';
	$birthdate = isset($_POST['birthdate']) ? mysqli_real_escape_string($_POST['birthdate']) : "";
	$firstname = isset($_POST['firstname']) ? mysqli_real_escape_string($_POST['firstname']) : "";
	$lastname = isset($_POST['lastname']) ? mysqli_real_escape_string($_POST['lastname']) : "";
	$sex = isset($_POST['sex']) ? mysqli_real_escape_string($_POST['sex']) : "";
	$tribeid = isset($_POST['clan']) ? mysqli_real_escape_string($_POST['clan']) : "";
	$mask = isset($_POST['mask']) ? mysqli_real_escape_string($_POST['mask']) : "";
	$mask_color = isset($_POST['maskcl']) ? mysqli_real_escape_string($_POST['maskcl']) : "";
	$lang_id = isset($_POST['lang_id']) ? addslashes($_POST['lang_id']) : 0;
	$error = '';
	 
   // get language suffix 
 	 if ($lang_id != 0) {
     $db->setQuery("SELECT * FROM `cc_extra_langs` WHERE id='{$lang_id}'");
     $res = $db->loadResult();
     $lang = "_".$res->lang;
   } else $lang = "";  

   $reg_ok = true;

   $db->setQuery("SELECT one_email_per_registration FROM `cc_def_settings`");
   $res = $db->loadResult();
   $one_registration_per_email = ($res->one_email_per_registration == 1);

   $email_check_ok = true;
   if ($one_registration_per_email == true) {
     $sql = "SELECT COUNT(*) AS counter FROM `cc_user` WHERE email='{$email}'"; // for several registrations per one email address -- no check  
	   $db->setQuery($sql);
	   $res1 = $db->loadResult();
	   $email_check_ok = $res1->counter == "0";
	 }
   if ($email_check_ok == false) { 
     $sql = "SELECT * FROM `cc_translations` WHERE caption='DUPLICATED_EMAIL'"; 
     $db->setQuery($sql);
  	 $res = $db->loadResult();
	   echo 'error='.urlencode($res->{"name".$lang});
	   $reg_ok = false;
   }

    /*if ($reg_ok && $email != '') {
      // get number of already registered number of registrations with this email address          
      $sql = "SELECT count(*) as registered_num_emails FROM `cc_user` WHERE email='{$email}'";
      $query = $db->setQuery($sql);
      $row = mysql_fetch_object($query);
      $registered_num_emails = $row->registered_num_emails;

      $sql = "SELECT max_num_account_per_email from `cc_def_settings`";
      $query = $db->setQuery($sql);
      $row = mysql_fetch_object($query);
      
      // it's possible to create new registration using this email address
      if ($registered_num_emails >= $row->max_num_account_per_email) {
        $sql = "SELECT * FROM `cc_translations` WHERE caption='MAX_NUM_REGISTRATION_REACHED'"; 
        $db->setQuery($sql);
      	$res = $db->loadResult();
    	  echo 'error='.urlencode($res->{"name".$lang});
        $reg_ok = false;
      }
    }*/
////////
//	   echo 'error=111';
//	   $reg_ok = false;
////////


	 if ($reg_ok) {
     // check for swear words     
  	 $db->setQuery("SELECT COUNT(*) as counter from `cc_swear_words` where INSTR('".$username."', `name`)");
  	 $res2 = $db->loadResult();
  	 if ((int)($res2->counter) > 0) { // swear word founded!
  	   $sql = "SELECT * FROM `cc_translations` WHERE caption='USERNAME_NOT_PERMITTED'";
       $db->setQuery($sql);
       $res = $db->loadResult();
       echo 'error='.urlencode($res->{"name".$lang});
       $reg_ok = false;
  	 }
   } 
	 
	 if ($reg_ok) {
     // first check there is no username with this name already registered.
  	 $db->setQuery("SELECT COUNT(*) AS counter FROM `cc_user` WHERE username='".$username."'");
  	 $res = $db->loadResult();
  	 if ((int)($res->counter) > 0) { // swear word founded!
       // get warning message from db
       $db->setQuery("SELECT * FROM `cc_translations` WHERE caption='USERNAME_IN_USE'");
       $res = $db->loadResult();
  	   echo 'error='.urlencode($res->{"name".$lang});
  	   $reg_ok = false;
	   }
   }
	 
   if ($reg_ok) echo 'result=true';	 
   
?>