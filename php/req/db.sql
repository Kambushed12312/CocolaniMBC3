-- phpMyAdmin SQL Dump
-- version 4.5.1
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Apr 16, 2016 at 03:26 PM
-- Server version: 10.1.10-MariaDB
-- PHP Version: 5.5.33

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `az3ar`
--

-- --------------------------------------------------------

--
-- Table structure for table `cc_bans`
--

CREATE TABLE `cc_bans` (
  `ip` varchar(255) NOT NULL,
  `reason` varchar(255) NOT NULL,
  `until` varchar(255) NOT NULL,
  `username` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_char_status`
--

CREATE TABLE `cc_char_status` (
  `is_min_level_home` varchar(255) NOT NULL,
  `display` varchar(255) NOT NULL,
  `ID` int(255) NOT NULL,
  `is_email_confirm_level` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_def_lang`
--

CREATE TABLE `cc_def_lang` (
  `lang` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_def_settings`
--

CREATE TABLE `cc_def_settings` (
  `caption` varchar(255) NOT NULL,
  `init_purse_amount` varchar(255) NOT NULL,
  `one_email_per_registration` varchar(255) NOT NULL,
  `ban_period_days` varchar(255) NOT NULL,
  `id` int(255) NOT NULL,
  `template_path` varchar(255) NOT NULL,
  `email_from` varchar(255) NOT NULL,
  `base_url` varchar(255) NOT NULL,
  `swf_url` varchar(255) NOT NULL,
  `reg_url` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `cc_def_settings`
--

INSERT INTO `cc_def_settings` (`caption`, `init_purse_amount`, `one_email_per_registration`, `ban_period_days`, `id`, `template_path`, `email_from`, `base_url`, `swf_url`, `reg_url`) VALUES
('', '', '1', '', 0, '', '', '', '', '');

-- --------------------------------------------------------

--
-- Table structure for table `cc_extra_langs`
--

CREATE TABLE `cc_extra_langs` (
  `id` int(255) NOT NULL,
  `lang` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_homes`
--

CREATE TABLE `cc_homes` (
  `user_id` int(255) NOT NULL,
  `street_num` varchar(255) NOT NULL,
  `tribe_ID` int(255) NOT NULL,
  `created` varchar(255) NOT NULL,
  `door_state` varchar(255) NOT NULL,
  `expiry_date` varchar(255) NOT NULL,
  `max_street` varchar(255) NOT NULL,
  `ID` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_homes_furniture`
--

CREATE TABLE `cc_homes_furniture` (
  `inv_id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_inv`
--

CREATE TABLE `cc_inv` (
  `active` varchar(255) NOT NULL,
  `user_id` int(255) NOT NULL,
  `ID` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_keywords`
--

CREATE TABLE `cc_keywords` (
  `id` int(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `enabled` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_mod_news`
--

CREATE TABLE `cc_mod_news` (
  `enabled` varchar(255) NOT NULL,
  `news_date` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_mod_news_keywords`
--

CREATE TABLE `cc_mod_news_keywords` (
  `news_id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_mod_news_settings`
--

CREATE TABLE `cc_mod_news_settings` (
  `id` int(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `date_format` varchar(255) NOT NULL,
  `news_date` varchar(255) NOT NULL,
  `news_date1` varchar(255) NOT NULL,
  `content` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_page`
--

CREATE TABLE `cc_page` (
  `id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_permissions`
--

CREATE TABLE `cc_permissions` (
  `user_id` int(255) NOT NULL,
  `char_status_id` int(255) NOT NULL,
  `id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_servers`
--

CREATE TABLE `cc_servers` (
  `server_id` int(255) NOT NULL,
  `id` int(255) NOT NULL,
  `lang_id` int(255) NOT NULL,
  `enabled` varchar(255) NOT NULL,
  `logins_active` varchar(255) NOT NULL,
  `zone_name` varchar(255) NOT NULL,
  `ip` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `cc_servers`
--

INSERT INTO `cc_servers` (`server_id`, `id`, `lang_id`, `enabled`, `logins_active`, `zone_name`, `ip`) VALUES
(0, 1, 0, '1', '', '', '');

-- --------------------------------------------------------

--
-- Table structure for table `cc_sessions`
--

CREATE TABLE `cc_sessions` (
  `username` varchar(255) NOT NULL,
  `session_start` datetime(6) NOT NULL,
  `session_end` datetime(6) NOT NULL,
  `IP` varchar(255) NOT NULL,
  `id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_stat`
--

CREATE TABLE `cc_stat` (
  `ip` varchar(255) NOT NULL,
  `time_diff` varchar(255) NOT NULL,
  `page` varchar(255) NOT NULL,
  `browser` varchar(255) NOT NULL,
  `os` varchar(255) NOT NULL,
  `host` varchar(255) NOT NULL,
  `referer` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_swear_words`
--

CREATE TABLE `cc_swear_words` (
  `name` varchar(255) NOT NULL,
  `username` varchar(255) NOT NULL,
  `add_Error` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_translations`
--

CREATE TABLE `cc_translations` (
  `caption` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `cc_translations`
--

INSERT INTO `cc_translations` (`caption`, `name`) VALUES
('REGFORM_PROBLEM', '');

-- --------------------------------------------------------

--
-- Table structure for table `cc_translations_system`
--

CREATE TABLE `cc_translations_system` (
  `friendly_name` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_tribes`
--

CREATE TABLE `cc_tribes` (
  `ID` int(255) NOT NULL,
  `newstr` varchar(255) NOT NULL,
  `init_purse_amount` varchar(255) NOT NULL,
  `active` varchar(255) NOT NULL,
  `total_population` varchar(255) NOT NULL,
  `tribe_id` int(255) NOT NULL,
  `population` varchar(255) NOT NULL,
  `battles_won` varchar(255) NOT NULL,
  `chief_id` int(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_user`
--

CREATE TABLE `cc_user` (
  `ID` int(255) NOT NULL,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `email` varchar(255) NOT NULL,
  `birth_date` date NOT NULL,
  `first_name` varchar(255) NOT NULL,
  `last_name` varchar(255) NOT NULL,
  `sex` varchar(255) NOT NULL,
  `about` varchar(255) NOT NULL,
  `mask` varchar(255) NOT NULL,
  `mask_colors` varchar(255) NOT NULL,
  `clothing` varchar(255) NOT NULL,
  `tribe_id` int(255) NOT NULL,
  `money` varchar(255) NOT NULL,
  `happyness` varchar(255) NOT NULL,
  `rank_ID` int(255) NOT NULL,
  `status_ID` int(255) NOT NULL,
  `lang_id` int(255) NOT NULL,
  `register_date` date NOT NULL,
  `uniqid` int(255) NOT NULL,
  `permission_id` int(255) NOT NULL,
  `primary_id` int(255) NOT NULL,
  `example` varchar(255) NOT NULL,
  `registered_num_emails` varchar(255) NOT NULL,
  `social_id` int(255) NOT NULL,
  `adr_1` varchar(255) NOT NULL,
  `adr_2` varchar(255) NOT NULL,
  `mobile` varchar(255) NOT NULL,
  `usercount` varchar(255) NOT NULL,
  `landline` varchar(255) NOT NULL,
  `registration_confirmed` varchar(255) NOT NULL,
  `won` varchar(255) NOT NULL,
  `loss` varchar(255) NOT NULL,
  `medals` varchar(255) NOT NULL,
  `lvl` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_userreginfo`
--

CREATE TABLE `cc_userreginfo` (
  `ID` int(255) NOT NULL,
  `user_id` int(255) NOT NULL,
  `register_IP` varchar(255) NOT NULL,
  `register_date` date NOT NULL,
  `last_update` date NOT NULL,
  `IP` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_user_actions_history`
--

CREATE TABLE `cc_user_actions_history` (
  `user_id` int(255) NOT NULL,
  `subject` int(255) NOT NULL,
  `subject_id` int(255) NOT NULL,
  `action` varchar(255) NOT NULL,
  `when` varchar(255) NOT NULL,
  `deleted_value` varchar(255) NOT NULL,
  `subject_field` varchar(255) NOT NULL,
  `old_value` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_user_parent`
--

CREATE TABLE `cc_user_parent` (
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `id` int(255) NOT NULL,
  `email` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `cc_zones`
--

CREATE TABLE `cc_zones` (
  `zone_name` varchar(255) NOT NULL,
  `friendly_name` varchar(255) NOT NULL,
  `port` varchar(255) NOT NULL,
  `activity_ratio` varchar(255) NOT NULL,
  `safe_chat` varchar(255) NOT NULL,
  `id` int(255) NOT NULL,
  `server_id` int(255) NOT NULL,
  `ip` varchar(255) NOT NULL,
  `lang_id` int(255) NOT NULL,
  `enabled` varchar(255) NOT NULL,
  `logins_active` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `cc_char_status`
--
ALTER TABLE `cc_char_status`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_def_settings`
--
ALTER TABLE `cc_def_settings`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_extra_langs`
--
ALTER TABLE `cc_extra_langs`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_homes`
--
ALTER TABLE `cc_homes`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_inv`
--
ALTER TABLE `cc_inv`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_keywords`
--
ALTER TABLE `cc_keywords`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_mod_news_settings`
--
ALTER TABLE `cc_mod_news_settings`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_page`
--
ALTER TABLE `cc_page`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_permissions`
--
ALTER TABLE `cc_permissions`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_servers`
--
ALTER TABLE `cc_servers`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_sessions`
--
ALTER TABLE `cc_sessions`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_tribes`
--
ALTER TABLE `cc_tribes`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_user`
--
ALTER TABLE `cc_user`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_userreginfo`
--
ALTER TABLE `cc_userreginfo`
  ADD PRIMARY KEY (`ID`);

--
-- Indexes for table `cc_user_parent`
--
ALTER TABLE `cc_user_parent`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `cc_zones`
--
ALTER TABLE `cc_zones`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `cc_char_status`
--
ALTER TABLE `cc_char_status`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_def_settings`
--
ALTER TABLE `cc_def_settings`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_extra_langs`
--
ALTER TABLE `cc_extra_langs`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_homes`
--
ALTER TABLE `cc_homes`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_inv`
--
ALTER TABLE `cc_inv`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_keywords`
--
ALTER TABLE `cc_keywords`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_mod_news_settings`
--
ALTER TABLE `cc_mod_news_settings`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_page`
--
ALTER TABLE `cc_page`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_permissions`
--
ALTER TABLE `cc_permissions`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_servers`
--
ALTER TABLE `cc_servers`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `cc_sessions`
--
ALTER TABLE `cc_sessions`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_tribes`
--
ALTER TABLE `cc_tribes`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_user`
--
ALTER TABLE `cc_user`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_userreginfo`
--
ALTER TABLE `cc_userreginfo`
  MODIFY `ID` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_user_parent`
--
ALTER TABLE `cc_user_parent`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `cc_zones`
--
ALTER TABLE `cc_zones`
  MODIFY `id` int(255) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
