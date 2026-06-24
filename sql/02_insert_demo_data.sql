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
-- Dumping data for table `storage_zones`
--

LOCK TABLES `storage_zones` WRITE;
/*!40000 ALTER TABLE `storage_zones` DISABLE KEYS */;
INSERT INTO `storage_zones` (`zone_id`, `zone_code`, `name`, `description`, `temp_min`, `temp_max`, `humidity_min`, `humidity_max`, `active`, `created_at`) VALUES (1,'AMBIENT','Zona ambient','Zona de conservacio ambient controlada',15.00,25.00,NULL,NULL,1,'2026-06-13 01:21:08'),(2,'COLD_CHAIN','Cambra freda','Zona de cadena de fred',2.00,8.00,NULL,NULL,1,'2026-06-13 01:21:08'),(3,'ROBOT_AREA','Zona robotitzada','Zona mecanica/robotitzada sense limits termics farmacologics directes',NULL,NULL,NULL,NULL,1,'2026-06-13 01:21:08');
/*!40000 ALTER TABLE `storage_zones` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping data for table `sensors`
--

LOCK TABLES `sensors` WRITE;
/*!40000 ALTER TABLE `sensors` DISABLE KEYS */;
INSERT INTO `sensors` (`sensor_id`, `sensor_code`, `sensor_type`, `unit`, `zone_id`, `active`, `created_at`) VALUES (1,'TEMP_AMB_01','temperature','degC',1,1,'2026-06-13 01:25:11'),(2,'HUM_AMB_01','humidity','%RH',1,1,'2026-06-13 01:25:11'),(3,'TEMP_COLD_01','temperature','degC',2,1,'2026-06-13 01:25:11'),(4,'VIB_ROBOT_01','vibration','mm_s',3,1,'2026-06-13 01:25:11');
/*!40000 ALTER TABLE `sensors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping data for table `alarm_rules`
--

LOCK TABLES `alarm_rules` WRITE;
/*!40000 ALTER TABLE `alarm_rules` DISABLE KEYS */;
INSERT INTO `alarm_rules` (`id`, `rule_code`, `sensor_code`, `priority`, `alarm_low`, `alarm_high`, `enabled`, `created_at`, `hysteresis_value`, `activation_delay_seconds`, `return_normal_delay_seconds`) VALUES (1,'RULE_TEMP_AMB_01','TEMP_AMB_01','HIGH',15.00,25.00,1,'2026-06-17 04:38:53',0.5,60,60),(2,'RULE_TEMP_COLD_01','TEMP_COLD_01','CRITICAL',2.00,8.00,1,'2026-06-17 04:38:53',0.5,60,60),(3,'RULE_HUM_AMB_01','HUM_AMB_01','MEDIUM',30.00,70.00,1,'2026-06-17 04:38:53',2,60,60),(4,'RULE_VIB_ROBOT_01','VIB_ROBOT_01','MEDIUM',0.00,5.00,1,'2026-06-17 04:38:53',0.5,15,30);
/*!40000 ALTER TABLE `alarm_rules` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-06-24 15:04:21
