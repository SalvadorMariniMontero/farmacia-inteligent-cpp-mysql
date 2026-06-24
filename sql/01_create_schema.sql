-- MySQL dump 10.13  Distrib 8.0.46, for Win64 (x86_64)
--
-- Host: 127.0.0.1    Database: farmacia_inteligente
-- ------------------------------------------------------
-- Server version	8.0.46

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `storage_zones`
--

DROP TABLE IF EXISTS `storage_zones`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `storage_zones` (
  `zone_id` int NOT NULL AUTO_INCREMENT,
  `zone_code` varchar(50) NOT NULL,
  `name` varchar(100) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `temp_min` decimal(6,2) DEFAULT NULL,
  `temp_max` decimal(6,2) DEFAULT NULL,
  `humidity_min` decimal(6,2) DEFAULT NULL,
  `humidity_max` decimal(6,2) DEFAULT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`zone_id`),
  UNIQUE KEY `zone_code` (`zone_code`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sensors`
--

DROP TABLE IF EXISTS `sensors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sensors` (
  `sensor_id` int NOT NULL AUTO_INCREMENT,
  `sensor_code` varchar(50) NOT NULL,
  `sensor_type` varchar(50) NOT NULL,
  `unit` varchar(20) NOT NULL,
  `zone_id` int NOT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`sensor_id`),
  UNIQUE KEY `sensor_code` (`sensor_code`),
  KEY `fk_sensors_storage_zones` (`zone_id`),
  CONSTRAINT `fk_sensors_storage_zones` FOREIGN KEY (`zone_id`) REFERENCES `storage_zones` (`zone_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `alarm_rules`
--

DROP TABLE IF EXISTS `alarm_rules`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `alarm_rules` (
  `id` int NOT NULL AUTO_INCREMENT,
  `rule_code` varchar(50) NOT NULL,
  `sensor_code` varchar(50) NOT NULL,
  `priority` varchar(20) NOT NULL,
  `alarm_low` decimal(10,2) DEFAULT NULL,
  `alarm_high` decimal(10,2) DEFAULT NULL,
  `enabled` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `hysteresis_value` double NOT NULL DEFAULT '0',
  `activation_delay_seconds` int NOT NULL DEFAULT '0',
  `return_normal_delay_seconds` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `rule_code` (`rule_code`),
  KEY `fk_alarm_rules_sensor` (`sensor_code`),
  CONSTRAINT `fk_alarm_rules_sensor` FOREIGN KEY (`sensor_code`) REFERENCES `sensors` (`sensor_code`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sensor_readings`
--

DROP TABLE IF EXISTS `sensor_readings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `sensor_readings` (
  `reading_id` int NOT NULL AUTO_INCREMENT,
  `sensor_id` int NOT NULL,
  `reading_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `reading_value` double NOT NULL,
  `source` varchar(80) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`reading_id`),
  KEY `idx_sensor_readings_sensor_time` (`sensor_id`,`reading_time`),
  KEY `idx_sensor_readings_source_time` (`source`,`reading_time`),
  CONSTRAINT `fk_sensor_readings_sensor` FOREIGN KEY (`sensor_id`) REFERENCES `sensors` (`sensor_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=222 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `alarm_events`
--

DROP TABLE IF EXISTS `alarm_events`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `alarm_events` (
  `alarm_event_id` bigint NOT NULL AUTO_INCREMENT,
  `rule_id` int NOT NULL,
  `reading_id` bigint DEFAULT NULL,
  `event_state` enum('ACTIVE','ACKNOWLEDGED','RETURNED','CLOSED') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'ACTIVE',
  `activation_time_utc` timestamp(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
  `activation_value` decimal(12,4) DEFAULT NULL,
  `acknowledged_time_utc` timestamp(3) NULL DEFAULT NULL,
  `acknowledged_by` varchar(80) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `closed_time_utc` timestamp(3) NULL DEFAULT NULL,
  `closed_by` varchar(80) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `comment` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `status` varchar(20) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'ACTIVE',
  `repeat_count` int NOT NULL DEFAULT '0',
  `alarm_value` decimal(10,3) NOT NULL DEFAULT '0.000',
  `alarm_direction` varchar(10) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `last_alarm_value` decimal(10,3) DEFAULT NULL,
  `triggered_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_seen_at` datetime DEFAULT NULL,
  `normal_since_at` datetime DEFAULT NULL,
  `returned_to_normal_at` datetime DEFAULT NULL,
  `return_normal_value` double DEFAULT NULL,
  PRIMARY KEY (`alarm_event_id`),
  KEY `fk_alarm_events_reading` (`reading_id`),
  KEY `idx_alarm_events_state_time` (`event_state`,`activation_time_utc`),
  KEY `idx_alarm_events_rule_time` (`rule_id`,`activation_time_utc`),
  KEY `idx_alarm_events_open_duplicates` (`rule_id`,`alarm_direction`,`status`)
) ENGINE=InnoDB AUTO_INCREMENT=59 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `audit_trail`
--

DROP TABLE IF EXISTS `audit_trail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `audit_trail` (
  `audit_id` bigint NOT NULL AUTO_INCREMENT,
  `table_name` varchar(100) NOT NULL,
  `record_id` bigint NOT NULL,
  `action` varchar(100) NOT NULL,
  `old_value` varchar(255) DEFAULT NULL,
  `new_value` varchar(255) DEFAULT NULL,
  `changed_by` varchar(100) NOT NULL,
  `changed_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`audit_id`),
  KEY `idx_audit_table_record` (`table_name`,`record_id`),
  KEY `idx_audit_action` (`action`),
  KEY `idx_audit_changed_at` (`changed_at`)
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `alarm_pending_conditions`
--

DROP TABLE IF EXISTS `alarm_pending_conditions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `alarm_pending_conditions` (
  `pending_id` int NOT NULL AUTO_INCREMENT,
  `sensor_code` varchar(50) NOT NULL,
  `rule_code` varchar(80) NOT NULL,
  `alarm_direction` varchar(10) NOT NULL,
  `first_abnormal_at` datetime NOT NULL,
  `last_abnormal_at` datetime NOT NULL,
  `last_reading_value` double NOT NULL,
  `pending_source` varchar(80) DEFAULT NULL,
  PRIMARY KEY (`pending_id`),
  UNIQUE KEY `uq_pending_alarm` (`sensor_code`,`rule_code`,`alarm_direction`),
  KEY `idx_pending_sensor` (`sensor_code`),
  KEY `idx_pending_rule` (`rule_code`),
  KEY `idx_pending_time` (`first_abnormal_at`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-06-24 15:02:03
