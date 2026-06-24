#include "Database.h"

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <map>

#include <utility>
#include <fstream>
#include <vector>
#include <direct.h>

namespace
{
    std::string csvEscape(const std::string& value)
    {
        bool needsQuotes =
            value.find(',')  != std::string::npos ||
            value.find('"')  != std::string::npos ||
            value.find('\n') != std::string::npos ||
            value.find('\r') != std::string::npos;

        std::string escaped;

        for (char c : value)
        {
            if (c == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += c;
            }
        }

        if (needsQuotes)
        {
            return "\"" + escaped + "\"";
        }

        return escaped;
    }

    std::string csvField(sql::ResultSet* res, const std::string& columnName)
    {
        if (res->isNull(columnName))
        {
            return "NULL";
        }

        return csvEscape(res->getString(columnName).c_str());
    }

    std::string htmlEscape(const std::string& value)
    {
        std::string escaped;

        for (char c : value)
        {
            switch (c)
            {
            case '&':
                escaped += "&amp;";
                break;
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            case '"':
                escaped += "&quot;";
                break;
            case '\'':
                escaped += "&#39;";
                break;
            default:
                escaped += c;
                break;
            }
        }

        return escaped;
    }

    std::string plainField(sql::ResultSet* res, const std::string& columnName)
    {
        if (res->isNull(columnName))
        {
            return "NULL";
        }

        return res->getString(columnName).c_str();
    }
}

Database::Database(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& schema
)
    : host_(host),
    user_(user),
    password_(password),
    schema_(schema),
    driver_(nullptr),
    connection_(nullptr)
{}

Database::~Database()
{
    disconnect();
}

bool Database::connect()
{
    try
    {
        driver_ = sql::mysql::get_mysql_driver_instance();
        connection_.reset(driver_->connect(host_, user_, password_));
        connection_->setSchema(schema_);

        std::cout << "[Database] Connexio MySQL correcta. Schema: "
          << schema_ << "\n";
        return true;
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en la connexio.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Database] ERROR general en la connexio.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        return false;
    }
}

bool Database::testVersion()
{
    if (!connection_)
    {
        std::cerr << "[Database] No hi ha connexio activa.\n";
        return false;
    }

    try
    {
        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT VERSION()"));
        
        if (res->next())
        {
            std::cout << "[Database] Versio MySQL: " << res->getString(1) << "\n";
        }

        return true;
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en testVersion().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::listStorageZones()
{
    if (!connection_)
    {
        std::cerr << "[Database] No hi ha connexio activa.\n";
        return false;
    }

    try
    {
        std::cout << "\n=== STORAGE_ZONES ===\n\n";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        const std::string query =
            "SELECT "
            "`zone_code`, "
            "`name`, "
            "COALESCE(CAST(`temp_min` AS CHAR), 'NULL') AS temp_min_txt, "
            "COALESCE(CAST(`temp_max` AS CHAR), 'NULL') AS temp_max_txt "
            "FROM `storage_zones` "
            "ORDER BY `zone_code`";

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        std::cout
            << std::left
            << std::setw(20) << "zone_code"
            << std::setw(25) << "name"
            << std::setw(15) << "temp_min"
            << std::setw(15) << "temp_max"
            << "\n";

        std::cout
            << std::setw(20) << "----------------"
            << std::setw(25) << "---------------------"
            << std::setw(15) << "----------"
            << std::setw(15) << "----------"
            << "\n";

        int count = 0;

        while (res->next())
        {
            std::cout
                << std::left
                << std::setw(20) << res->getString("zone_code")
                << std::setw(25) << res->getString("name")
                << std::setw(15) << res->getString("temp_min_txt")
                << std::setw(15) << res->getString("temp_max_txt")
                << "\n";

            ++count;
        }

        std::cout << "\nZones llegides: " << count << "\n";

        return true;
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en listStorageZones().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Database] ERROR general en listStorageZones().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        return false;
    }
}

bool Database::listSensors()
{
    if (!connection_)
    {
        std::cerr << "[Database] No hi ha connexio activa.\n";
        return false;
    }

    try
    {
        std::cout << "\n=== SENSORS ===\n\n";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        const std::string query =
            "SELECT "
            "s.`sensor_code`, "
            "s.`sensor_type`, "
            "s.`unit`, "
            "z.`zone_code`, "
            "CAST(s.`active` AS CHAR) AS active_txt "
            "FROM `sensors` s "
            "JOIN `storage_zones` z ON s.`zone_id` = z.`zone_id` "
            "ORDER BY s.`sensor_code`";

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        std::cout
            << std::left
            << std::setw(20) << "sensor_code"
            << std::setw(20) << "sensor_type"
            << std::setw(12) << "unit"
            << std::setw(18) << "zone_code"
            << std::setw(10) << "active"
            << "\n";

        std::cout
            << std::setw(20) << "----------------"
            << std::setw(20) << "----------------"
            << std::setw(12) << "--------"
            << std::setw(18) << "--------------"
            << std::setw(10) << "------"
            << "\n";

        int count = 0;

        while (res->next())
        {
            std::cout
                << std::left
                << std::setw(20) << res->getString("sensor_code")
                << std::setw(20) << res->getString("sensor_type")
                << std::setw(12) << res->getString("unit")
                << std::setw(18) << res->getString("zone_code")
                << std::setw(10) << res->getString("active_txt")
                << "\n";

            ++count;
        }

        std::cout << "\nSensors llegits: " << count << "\n";

        return true;
    }
    catch (const sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en listSensors().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Database] ERROR general en listSensors().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        return false;
    }
}

namespace
{
    std::string decimalOrNull(sql::ResultSet* res, const std::string& columnName)
    {
        if (res->isNull(columnName))
        {
            return "NULL";
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << res->getDouble(columnName);
        return oss.str();
    }
}

bool Database::printAlarmRules()
{
    try
    {
        std::cout << "\n=== ALARM_RULES ===\n\n";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery(
                "SELECT rule_code, priority, alarm_low, alarm_high, enabled "
                "FROM alarm_rules "
                "ORDER BY rule_code"
            )
        );

        std::cout << std::left
            << std::setw(22) << "rule_code"
            << std::setw(14) << "priority"
            << std::setw(14) << "alarm_low"
            << std::setw(14) << "alarm_high"
            << std::setw(10) << "enabled"
            << "\n";

        std::cout << std::string(74, '-') << "\n";

        int count = 0;

        while (res->next())
        {
            std::cout << std::left
                << std::setw(22) << res->getString("rule_code")
                << std::setw(14) << res->getString("priority");

            if (res->isNull("alarm_low"))
                std::cout << std::setw(14) << "NULL";
            else
                std::cout << std::setw(14) << res->getDouble("alarm_low");

            if (res->isNull("alarm_high"))
                std::cout << std::setw(14) << "NULL";
            else
                std::cout << std::setw(14) << res->getDouble("alarm_high");

            std::cout << std::setw(10) << res->getInt("enabled")
                << "\n";

            count++;
        }

        std::cout << "\nRegles llegides: " << count << "\n";

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL llegint alarm_rules.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
    catch (std::exception& e)
    {
        std::cerr << "[Database] ERROR general llegint alarm_rules.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        return false;
    }
}

bool Database::insertReadingBySensorCode(
    const std::string& sensorCode,
    double readingValue,
    const std::string& source
) {
    if (!connection_) {
        std::cout << "[Database] No hi ha connexio activa.\n";
        return false;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> findSensorStmt(
            connection_->prepareStatement(
                "SELECT sensor_id FROM sensors "
                "WHERE sensor_code = ? AND active = 1 "
                "LIMIT 1"
            )
        );

        findSensorStmt->setString(1, sensorCode);

        std::unique_ptr<sql::ResultSet> rs(findSensorStmt->executeQuery());

        if (!rs->next()) {
            std::cout << "[5J] Sensor no trobat o inactiu: "
                      << sensorCode << "\n";
            return false;
        }

        int sensorId = rs->getInt("sensor_id");

        std::unique_ptr<sql::PreparedStatement> insertStmt(
            connection_->prepareStatement(
                "INSERT INTO sensor_readings "
                "(sensor_id, reading_time, reading_value, source) "
                "VALUES (?, NOW(), ?, ?)"
            )
        );

        insertStmt->setInt(1, sensorId);
        insertStmt->setDouble(2, readingValue);
        insertStmt->setString(3, source);

        int rows = insertStmt->executeUpdate();

        if (rows == 1) {
            std::cout << "[5J] Lectura inserida: "
                      << sensorCode << " = "
                      << readingValue
                      << " | source = " << source << "\n";
            return true;
        }

        std::cout << "[5J] No s'ha inserit cap fila per al sensor: "
                  << sensorCode << "\n";
        return false;
    }
    catch (const sql::SQLException& e) {
        std::cout << "[Database] ERROR SQL inserint lectura.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}


bool Database::insertSimulatedReadings5J() {
    std::cout << "\n=== FASE 5J - INSERCIO DE LECTURES SIMULADES ===\n\n";

    const std::string source = "cpp_fita_5J";

    bool ok = true;

    ok = insertReadingBySensorCode("TEMP_AMB_01", 22.30, source) && ok;
    ok = insertReadingBySensorCode("HUM_AMB_01", 45.00, source) && ok;
    ok = insertReadingBySensorCode("TEMP_COLD_01", 5.20, source) && ok;
    ok = insertReadingBySensorCode("VIB_ROBOT_01", 0.80, source) && ok;

    if (ok) {
        std::cout << "\n[5J] Totes les lectures simulades s'han inserit correctament.\n";
    } else {
        std::cout << "\n[5J] Alguna lectura no s'ha pogut inserir.\n";
    }

    return ok;
}


bool Database::printLatestSensorReadings5J(int limit) {
    if (!connection_) {
        std::cout << "[Database] No hi ha connexio activa.\n";
        return false;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> stmt(
            connection_->prepareStatement(
                "SELECT "
                "sr.reading_id, "
                "sr.reading_time, "
                "s.sensor_code, "
                "s.sensor_type, "
                "s.unit, "
                "sr.reading_value, "
                "sr.source "
                "FROM sensor_readings sr "
                "JOIN sensors s ON sr.sensor_id = s.sensor_id "
                "WHERE sr.source = ? "
                "ORDER BY sr.reading_time DESC, sr.reading_id DESC "
                "LIMIT ?"
            )
        );

        stmt->setString(1, "cpp_fita_5J");
        stmt->setInt(2, limit);

        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());

        std::cout << "\n=== ULTIMES LECTURES SENSOR_READINGS - FASE 5J ===\n\n";

        std::cout << std::left
                  << std::setw(8)  << "ID"
                  << std::setw(22) << "reading_time"
                  << std::setw(18) << "sensor_code"
                  << std::setw(16) << "sensor_type"
                  << std::setw(10) << "unit"
                  << std::setw(14) << "value"
                  << std::setw(16) << "source"
                  << "\n";

        std::cout << std::string(104, '-') << "\n";

        int count = 0;

        while (rs->next()) {
            ++count;

            std::cout << std::left
                      << std::setw(8)  << rs->getInt("reading_id")
                      << std::setw(22) << rs->getString("reading_time")
                      << std::setw(18) << rs->getString("sensor_code")
                      << std::setw(16) << rs->getString("sensor_type")
                      << std::setw(10) << rs->getString("unit")
                      << std::setw(14) << rs->getDouble("reading_value")
                      << std::setw(16) << rs->getString("source")
                      << "\n";
        }

        std::cout << "\nLectures recuperades: " << count << "\n";

        return true;
    }
    catch (const sql::SQLException& e) {
        std::cout << "[Database] ERROR SQL llegint lectures 5J.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::insertOutOfRangeReadingsFase5K()
{
    try
    {
        if (!connection_)
        {
            std::cout << "[5K] Connexio no inicialitzada.\n";
            return false;
        }

        std::cout << "\n=== FASE 5K - Insercio de lectures fora de rang ===\n";

        connection_->setAutoCommit(false);

        // Neteja controlada per poder repetir la prova sense duplicats.
        {
            std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

            stmt->executeUpdate(
                "DELETE ae FROM alarm_events ae "
                "JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
                "WHERE sr.source = 'cpp_fita_5K'"
            );

            stmt->executeUpdate(
                "DELETE FROM sensor_readings "
                "WHERE source = 'cpp_fita_5K'"
            );
        }

        const std::vector<std::pair<std::string, double>> readings = {
            {"TEMP_AMB_01",   28.7},  // AMBIENT: per sobre de 25 C
            {"HUM_AMB_01",    78.0},  // Humitat alta, segons regla existent
            {"TEMP_COLD_01",  10.5},  // COLD_CHAIN: per sobre de 8 C
            {"VIB_ROBOT_01",   6.2}   // Vibracio alta, segons regla existent
        };

        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "INSERT INTO sensor_readings "
                "(sensor_id, reading_value, reading_time, source) "
                "SELECT sensor_id, ?, NOW(), 'cpp_fita_5K' "
                "FROM sensors "
                "WHERE sensor_code = ?"
            )
        );

        int inserted = 0;

        for (const auto& item : readings)
        {
            const std::string& sensorCode = item.first;
            double value = item.second;

            pstmt->setDouble(1, value);
            pstmt->setString(2, sensorCode);

            int rows = pstmt->executeUpdate();
            inserted += rows;

            std::cout << "Lectura inserida: "
                      << std::left << std::setw(15) << sensorCode
                      << " = " << value
                      << "   rows=" << rows << "\n";
        }

        connection_->commit();
        connection_->setAutoCommit(true);

        std::cout << "Lectures fora de rang inserides: " << inserted << "\n";
        return inserted == 4;
    }
    catch (sql::SQLException& e)
    {
        try
        {
            if (connection_)
            {
                connection_->rollback();
                connection_->setAutoCommit(true);
            }
        }
        catch (...) {}

        std::cout << "[5K] ERROR SQL inserint lectures fora de rang.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::evaluateReadingsAndCreateAlarmsFase5K()
{
    try
    {
        if (!connection_)
        {
            std::cout << "[5K] Connexio no inicialitzada.\n";
            return false;
        }

        std::cout << "\n=== FASE 5K - Avaluacio de regles i creacio d'alarmes ACTIVE ===\n";

        const std::string query =
            "SELECT "
            "sr.reading_id AS reading_id, "
            "s.sensor_code, "
            "sr.reading_value, "
            "ar.id AS rule_id, "
            "ar.rule_code, "
            "ar.priority, "
            "ar.alarm_low, "
            "ar.alarm_high "
            "FROM sensor_readings sr "
            "JOIN sensors s ON sr.sensor_id = s.sensor_id "
            "JOIN alarm_rules ar ON ar.rule_code = CONCAT('RULE_', s.sensor_code) "
            "WHERE sr.source = 'cpp_fita_5K' "
            "AND ar.enabled = 1 "
            "ORDER BY sr.reading_id, ar.rule_code";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(query));

        std::unique_ptr<sql::PreparedStatement> insertAlarm(
            connection_->prepareStatement(
                "INSERT INTO alarm_events "
                "(rule_id, reading_id, status, alarm_value, triggered_at) "
                "VALUES (?, ?, 'ACTIVE', ?, NOW())"
            )
        );

        int evaluated = 0;
        int created = 0;

        while (rs->next())
        {
            ++evaluated;

            int readingId = rs->getInt("reading_id");
            int ruleId = rs->getInt("rule_id");
            std::string sensorCode = rs->getString("sensor_code");
            std::string ruleCode = rs->getString("rule_code");
            std::string priority = rs->getString("priority");
            double value = rs->getDouble("reading_value");

            bool hasLow = !rs->isNull("alarm_low");
            bool hasHigh = !rs->isNull("alarm_high");

            double alarmLow = hasLow ? rs->getDouble("alarm_low") : 0.0;
            double alarmHigh = hasHigh ? rs->getDouble("alarm_high") : 0.0;

            bool lowAlarm = hasLow && (value < alarmLow);
            bool highAlarm = hasHigh && (value > alarmHigh);

            if (lowAlarm || highAlarm)
            {
                insertAlarm->setInt(1, ruleId);
                insertAlarm->setInt(2, readingId);
                insertAlarm->setDouble(3, value);

                int rows = insertAlarm->executeUpdate();
                created += rows;

                std::cout << "ALARMA ACTIVE creada: "
                          << std::left << std::setw(15) << sensorCode
                          << " regla=" << std::setw(18) << ruleCode
                          << " valor=" << std::setw(8) << value
                          << " prioritat=" << priority
                          << "\n";
            }
            else
            {
                std::cout << "Sense alarma: "
                          << std::left << std::setw(15) << sensorCode
                          << " regla=" << ruleCode
                          << " valor=" << value
                          << "\n";
            }
        }

        std::cout << "\nRegles avaluades: " << evaluated << "\n";
        std::cout << "Alarmes ACTIVE creades: " << created << "\n";

        return created > 0;
    }
    catch (sql::SQLException& e)
    {
        std::cout << "[5K] ERROR SQL avaluant regles d'alarma.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::printAlarmsFase5K()
{
    try
    {
        if (!connection_)
        {
            std::cout << "[5K] Connexio no inicialitzada.\n";
            return false;
        }

        std::cout << "\n=== ALARM_EVENTS GENERADES A LA FASE 5K ===\n\n";

        const std::string query =
            "SELECT "
            "s.sensor_code, "
            "ar.rule_code, "
            "ar.priority, "
            "sr.reading_value, "
            "ar.alarm_low, "
            "ar.alarm_high, "
            "ae.status, "
            "ae.triggered_at "
            "FROM alarm_events ae "
            "JOIN alarm_rules ar ON ae.rule_id = ar.id "
            "JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
            "JOIN sensors s ON sr.sensor_id = s.sensor_id "
            "WHERE sr.source = 'cpp_fita_5K' "
            "ORDER BY ae.triggered_at, s.sensor_code";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(query));

        std::cout << std::left
            << std::setw(16) << "sensor"
            << std::setw(20) << "rule"
            << std::setw(12) << "priority"
            << std::setw(12) << "value"
            << std::setw(12) << "low"
            << std::setw(12) << "high"
            << std::setw(14) << "status"
            << "triggered_at"
            << "\n";

        std::cout << std::string(120, '-') << "\n";

        int count = 0;

        while (rs->next())
        {
            ++count;

            std::cout << std::left
                      << std::setw(16) << rs->getString("sensor_code")
                      << std::setw(20) << rs->getString("rule_code")
                      << std::setw(12) << rs->getString("priority")
                      << std::setw(12) << rs->getDouble("reading_value");

            if (rs->isNull("alarm_low"))
                std::cout << std::setw(12) << "NULL";
            else
                std::cout << std::setw(12) << rs->getDouble("alarm_low");

            if (rs->isNull("alarm_high"))
                std::cout << std::setw(12) << "NULL";
            else
                std::cout << std::setw(12) << rs->getDouble("alarm_high");

            std::cout << std::setw(14) << rs->getString("status")
                      << rs->getString("triggered_at")
                      << "\n";
        }

        std::cout << "\nAlarmes mostrades: " << count << "\n";
        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cout << "[5K] ERROR SQL mostrant alarmes 5K.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::insertAuditTrailFase5L(
    const std::string& tableName,
    long long recordId,
    const std::string& action,
    const std::string& oldValue,
    const std::string& newValue,
    const std::string& changedBy
) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "INSERT INTO audit_trail "
                "(table_name, record_id, action, old_value, new_value, changed_by, changed_at) "
                "VALUES (?, ?, ?, ?, ?, ?, NOW())"
            )
        );

        pstmt->setString(1, tableName);
        pstmt->setInt64(2, recordId);
        pstmt->setString(3, action);
        pstmt->setString(4, oldValue);
        pstmt->setString(5, newValue);
        pstmt->setString(6, changedBy);

        int rows = pstmt->executeUpdate();

        return rows == 1;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Fase 5L] ERROR SQL insertant audit_trail.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::acknowledgeActiveAlarmsFase5L(const std::string& user) {
    try {
        std::cout << "\n=== FASE 5L - Reconeixement d'alarmes ACTIVE ===\n";

        connection_->setAutoCommit(false);

        std::unique_ptr<sql::PreparedStatement> selectStmt(
            connection_->prepareStatement(
                "SELECT ae.alarm_event_id, ae.rule_id, s.sensor_code "
                "FROM alarm_events ae "
                "JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
                "JOIN sensors s ON sr.sensor_id = s.sensor_id "
                "WHERE ae.status = 'ACTIVE' "
                "AND sr.source = 'cpp_fita_5K' "
                "ORDER BY ae.alarm_event_id"
            )
        );

        std::unique_ptr<sql::ResultSet> rs(selectStmt->executeQuery());

        int ackCount = 0;

       while (rs->next()) {
            long long alarmEventId = rs->getInt64("alarm_event_id");
            std::string sensorCode = rs->getString("sensor_code");
            int ruleId = rs->getInt("rule_id");

            std::unique_ptr<sql::PreparedStatement> updateStmt(
                connection_->prepareStatement(
                    "UPDATE alarm_events "
                    "SET status = 'ACKNOWLEDGED', "
                    "    acknowledged_time_utc = UTC_TIMESTAMP(3), "
                    "    acknowledged_by = ? "
                    "WHERE alarm_event_id = ? "
                    "AND status = 'ACTIVE'"
                )
            );

            updateStmt->setString(1, user);
            updateStmt->setInt64(2, alarmEventId);
            int rows = updateStmt->executeUpdate();

            if (rows == 1) {
                bool auditOk = insertAuditTrailFase5L(
                    "alarm_events",
                    alarmEventId,
                    "ACK_ALARM",
                    "ACTIVE",
                    "ACKNOWLEDGED",
                    user
                );

                if (!auditOk) {
                    throw sql::SQLException("No s'ha pogut inserir audit_trail ACK_ALARM");
                }

                std::cout << "ACK_ALARM: alarm_event_id=" << alarmEventId
                            << " sensor=" << sensorCode
                            << " rule_id=" << ruleId
                            << " user=" << user << "\n";

                ackCount++;
            }
        }

        connection_->commit();
        connection_->setAutoCommit(true);

        std::cout << "Alarmes reconegudes: " << ackCount << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        try {
            connection_->rollback();
            connection_->setAutoCommit(true);
        }
        catch (...) {}

        std::cerr << "[Fase 5L] ERROR SQL reconeixent alarmes.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";

        return false;
    }
}

bool Database::closeAcknowledgedAlarmsFase5L(const std::string& user) {
    try {
        std::cout << "\n=== FASE 5L - Tancament d'alarmes ACKNOWLEDGED ===\n";

        connection_->setAutoCommit(false);

        std::unique_ptr<sql::PreparedStatement> selectStmt(
            connection_->prepareStatement(
                "SELECT ae.alarm_event_id, ae.rule_id, s.sensor_code "
                "FROM alarm_events ae "
                "JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
                "JOIN sensors s ON sr.sensor_id = s.sensor_id "
                "WHERE ae.status = 'ACKNOWLEDGED' "
                "AND sr.source = 'cpp_fita_5K' "
                "ORDER BY ae.alarm_event_id"
            )
        );

        std::unique_ptr<sql::ResultSet> rs(selectStmt->executeQuery());

        int closeCount = 0;

       while (rs->next()) {
            long long alarmEventId = rs->getInt64("alarm_event_id");
            std::string sensorCode = rs->getString("sensor_code");
            int ruleId = rs->getInt("rule_id");

            std::unique_ptr<sql::PreparedStatement> updateStmt(
                connection_->prepareStatement(
                    "UPDATE alarm_events "
                    "SET status = 'CLOSED', "
                    "    closed_time_utc = UTC_TIMESTAMP(3), "
                    "    closed_by = ? "
                    "WHERE alarm_event_id = ? "
                    "AND status = 'ACKNOWLEDGED'"
                )
            );

            updateStmt->setString(1, user);
            updateStmt->setInt64(2, alarmEventId);

            int rows = updateStmt->executeUpdate();

            if (rows == 1) {
                bool auditOk = insertAuditTrailFase5L(
                    "alarm_events",
                    alarmEventId,
                    "CLOSE_ALARM",
                    "ACKNOWLEDGED",
                    "CLOSED",
                    user
                );

                if (!auditOk) {
                    throw sql::SQLException("No s'ha pogut inserir audit_trail CLOSE_ALARM");
                }

                std::cout << "CLOSE_ALARM: alarm_event_id=" << alarmEventId
                            << " sensor=" << sensorCode
                            << " rule_id=" << ruleId
                            << " user=" << user << "\n";

                closeCount++;
            }
        }

        connection_->commit();
        connection_->setAutoCommit(true);

        std::cout << "Alarmes tancades: " << closeCount << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        try {
            connection_->rollback();
            connection_->setAutoCommit(true);
        }
        catch (...) {}

        std::cerr << "[Fase 5L] ERROR SQL tancant alarmes.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";

        return false;
    }
}

bool Database::printAlarmLifecycleFase5L() {
    try {
        std::cout << "\n=== FASE 5L - Estat final de les alarmes ===\n\n";

        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "SELECT ae.alarm_event_id, "
                "       s.sensor_code, "
                "       ar.id AS rule_id, "
                "       ar.rule_code, "
                "       ar.priority, "
                "       ae.activation_value, "
                "       ae.alarm_value, "
                "       ae.status, "
                "       ae.triggered_at, "
                "       ae.acknowledged_time_utc, "
                "       ae.acknowledged_by, "
                "       ae.closed_time_utc, "
                "       ae.closed_by "
                "FROM alarm_events ae "
                "JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
                "JOIN sensors s ON sr.sensor_id = s.sensor_id "
                "JOIN alarm_rules ar ON ae.rule_id = ar.id "
                "WHERE sr.source = 'cpp_fita_5K' "
                "ORDER BY ae.alarm_event_id"
            )
        );

        std::unique_ptr<sql::ResultSet> rs(pstmt->executeQuery());

        std::cout << std::left
                << std::setw(18) << "alarm_event_id"
                << std::setw(18) << "sensor"
                << std::setw(10) << "rule_id"
                << std::setw(22) << "rule_code"
                << std::setw(12) << "priority"
                << std::setw(12) << "valor"
                << std::setw(16) << "status"
                << std::setw(18) << "ack_by"
                << std::setw(18) << "closed_by"
                << "\n";

        std::cout << std::string(114, '-') << "\n";

        int count = 0;

        while (rs->next()) {
            std::cout << std::left
                    << std::setw(18) << rs->getInt64("alarm_event_id")
                    << std::setw(18) << rs->getString("sensor_code")
                    << std::setw(10) << rs->getInt("rule_id")
                    << std::setw(22) << rs->getString("rule_code")
                    << std::setw(12) << rs->getString("priority")
                    << std::setw(12) << rs->getDouble("alarm_value")
                    << std::setw(16) << rs->getString("status")
                    << std::setw(18) << rs->getString("acknowledged_by")
                    << std::setw(18) << rs->getString("closed_by")
                    << "\n";

            count++;
        }

        std::cout << "\nAlarmes Fase 5K mostrades: " << count << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Fase 5L] ERROR SQL imprimint estat final.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";

        return false;
    }
}

bool Database::printAuditTrailFase5L() {
    try {
        std::cout << "\n=== FASE 5L - Audit trail generat ===\n\n";

        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "SELECT audit_id, table_name, record_id, action, "
                "       old_value, new_value, changed_by, changed_at "
                "FROM audit_trail "
                "WHERE table_name = 'alarm_events' "
                "AND action IN ('ACK_ALARM', 'CLOSE_ALARM') "
                "ORDER BY audit_id DESC "
                "LIMIT 20"
            )
        );

        std::unique_ptr<sql::ResultSet> rs(pstmt->executeQuery());

        std::cout << std::left
                  << std::setw(10) << "audit_id"
                  << std::setw(14) << "record_id"
                  << std::setw(16) << "action"
                  << std::setw(18) << "old_value"
                  << std::setw(18) << "new_value"
                  << std::setw(16) << "changed_by"
                  << "\n";

        std::cout << std::string(92, '-') << "\n";

        int count = 0;

        while (rs->next()) {
            std::cout << std::left
                      << std::setw(10) << rs->getInt64("audit_id")
                      << std::setw(14) << rs->getInt64("record_id")
                      << std::setw(16) << rs->getString("action")
                      << std::setw(18) << rs->getString("old_value")
                      << std::setw(18) << rs->getString("new_value")
                      << std::setw(16) << rs->getString("changed_by")
                      << "\n";

            count++;
        }

        std::cout << "\nRegistres audit_trail mostrats: " << count << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Fase 5L] ERROR SQL imprimint audit_trail.\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";

        return false;
    }
}

int Database::findOpenAlarmEventId(int ruleId, const std::string& alarmDirection)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "SELECT alarm_event_id "
                "FROM alarm_events "
                "WHERE rule_id = ? "
                "  AND alarm_direction = ? "
                "  AND status IN ('ACTIVE', 'ACKNOWLEDGED') "
                "ORDER BY activation_time_utc DESC, alarm_event_id DESC "
                "LIMIT 1"
            )
        );

        pstmt->setInt(1, ruleId);
        pstmt->setString(2, alarmDirection);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            return res->getInt("alarm_event_id");
        }

        return 0;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en findOpenAlarmEventId().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return -1;
    }
}

bool Database::updateRepeatedAlarm(int alarmEventId, double currentValue)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "UPDATE alarm_events "
                "SET repeat_count = repeat_count + 1, "
                "    last_seen_at = NOW(), "
                "    last_alarm_value = ?, "
                "    comment = 'Fase 5M: alarma repetida controlada' "
                "WHERE alarm_event_id = ?"
            )
        );

        pstmt->setDouble(1, currentValue);
        pstmt->setInt(2, alarmEventId);

        int rows = pstmt->executeUpdate();

        return rows > 0;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en updateRepeatedAlarm().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

int Database::createActiveAlarmEvent5M(
    int ruleId,
    int readingId,
    double alarmValue,
    const std::string& alarmDirection
)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            connection_->prepareStatement(
                "INSERT INTO alarm_events "
                "(rule_id, reading_id, event_state, status, activation_value, "
                " alarm_value, alarm_direction, last_alarm_value, comment) "
                "VALUES (?, ?, 'ACTIVE', 'ACTIVE', ?, ?, ?, ?, "
                " 'Fase 5M: alarma ACTIVE creada amb control de duplicats')"
            )
        );

        pstmt->setInt(1, ruleId);
        pstmt->setInt(2, readingId);
        pstmt->setDouble(3, alarmValue);
        pstmt->setDouble(4, alarmValue);
        pstmt->setString(5, alarmDirection);
        pstmt->setDouble(6, alarmValue);

        int rows = pstmt->executeUpdate();

        if (rows <= 0)
        {
            return 0;
        }

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT LAST_INSERT_ID() AS new_alarm_event_id")
        );

        if (res->next())
        {
            return res->getInt("new_alarm_event_id");
        }

        return 0;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en createActiveAlarmEvent5M().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return -1;
    }
}

bool Database::runFase5M()
{
    try
    {
        std::cout << "\n=== FASE 5M - Insercio de lectures fora de rang amb control de duplicats ===\n";

        struct TestReading
        {
            std::string sensorCode;
            double value;
        };

        std::vector<TestReading> readings = {
            {"TEMP_AMB_01", 28.7},
            {"HUM_AMB_01", 78.0},
            {"TEMP_COLD_01", 10.5},
            {"VIB_ROBOT_01", 6.2}
        };

        int insertedReadings = 0;
        int evaluatedRules = 0;
        int createdAlarms = 0;
        int repeatedAlarms = 0;

        for (const auto& item : readings)
        {
            int readingId = 0;

            {
                std::unique_ptr<sql::PreparedStatement> pstmt(
                    connection_->prepareStatement(
                        "INSERT INTO sensor_readings "
                        "(sensor_id, reading_value, source) "
                        "SELECT sensor_id, ?, 'cpp_fita_5M' "
                        "FROM sensors "
                        "WHERE sensor_code = ?"
                    )
                );

                pstmt->setDouble(1, item.value);
                pstmt->setString(2, item.sensorCode);

                int rows = pstmt->executeUpdate();

                if (rows != 1)
                {
                    std::cerr << "[5M] No s'ha pogut inserir lectura per sensor: "
                              << item.sensorCode << "\n";
                    continue;
                }

                insertedReadings++;

                std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
                std::unique_ptr<sql::ResultSet> res(
                    stmt->executeQuery("SELECT LAST_INSERT_ID() AS reading_id")
                );

                if (res->next())
                {
                    readingId = res->getInt("reading_id");
                }
            }

            std::cout << "[5M] Lectura inserida: "
                      << item.sensorCode << " = " << item.value
                      << " | reading_id=" << readingId
                      << " | source=cpp_fita_5M\n";

            if (readingId <= 0)
            {
                continue;
            }

            std::unique_ptr<sql::PreparedStatement> pstmtRule(
                connection_->prepareStatement(
                    "SELECT "
                    "  sr.reading_id, "
                    "  s.sensor_code, "
                    "  ar.id AS rule_id, "
                    "  ar.rule_code, "
                    "  ar.priority, "
                    "  ar.alarm_low, "
                    "  ar.alarm_high, "
                    "  sr.reading_value AS reading_value "
                    "FROM sensor_readings sr "
                    "JOIN sensors s ON s.sensor_id = sr.sensor_id "
                    "JOIN alarm_rules ar ON ar.sensor_code = s.sensor_code "
                    "WHERE sr.reading_id = ? "
                    "  AND ar.enabled = 1"
                )
            );

            pstmtRule->setInt(1, readingId);

            std::unique_ptr<sql::ResultSet> resRule(pstmtRule->executeQuery());

            while (resRule->next())
            {
                evaluatedRules++;

                int ruleId = resRule->getInt("rule_id");
                std::string sensorCode = resRule->getString("sensor_code");
                std::string ruleCode = resRule->getString("rule_code");
                std::string priority = resRule->getString("priority");

                double currentValue = resRule->getDouble("reading_value");

                bool hasLow = !resRule->isNull("alarm_low");
                bool hasHigh = !resRule->isNull("alarm_high");

                double alarmLow = hasLow ? resRule->getDouble("alarm_low") : 0.0;
                double alarmHigh = hasHigh ? resRule->getDouble("alarm_high") : 0.0;

                std::string alarmDirection;

                if (hasHigh && currentValue > alarmHigh)
                {
                    alarmDirection = "HIGH";
                }
                else if (hasLow && currentValue < alarmLow)
                {
                    alarmDirection = "LOW";
                }
                else
                {
                    std::cout << "[5M] Lectura dins de rang: "
                              << sensorCode << " valor=" << currentValue << "\n";
                    continue;
                }

                int openAlarmEventId = findOpenAlarmEventId(ruleId, alarmDirection);

                if (openAlarmEventId > 0)
                {
                    if (updateRepeatedAlarm(openAlarmEventId, currentValue))
                    {
                        repeatedAlarms++;

                        std::cout << "ALARMA REPETIDA CONTROLADA: "
                                  << "alarm_event_id=" << openAlarmEventId
                                  << " sensor=" << sensorCode
                                  << " regla=" << ruleCode
                                  << " direccio=" << alarmDirection
                                  << " valor=" << currentValue
                                  << "\n";
                    }
                }
                else if (openAlarmEventId == 0)
                {
                    int newAlarmEventId = createActiveAlarmEvent5M(
                        ruleId,
                        readingId,
                        currentValue,
                        alarmDirection
                    );

                    if (newAlarmEventId > 0)
                    {
                        createdAlarms++;

                        std::cout << "ALARMA ACTIVE creada: "
                                  << "alarm_event_id=" << newAlarmEventId
                                  << " sensor=" << sensorCode
                                  << " regla=" << ruleCode
                                  << " prioritat=" << priority
                                  << " direccio=" << alarmDirection
                                  << " valor=" << currentValue
                                  << "\n";
                    }
                    else
                    {
                        std::cerr << "[5M] Error creant alarma ACTIVE per regla "
                                  << ruleCode << "\n";
                    }
                }
                else
                {
                    std::cerr << "[5M] Error comprovant duplicat per rule_id="
                              << ruleId
                              << " direction=" << alarmDirection
                              << "\n";
                }
            }
        }

        std::cout << "\n=== RESUM FASE 5M ===\n";
        std::cout << "Lectures fora de rang inserides: " << insertedReadings << "\n";
        std::cout << "Regles avaluades: " << evaluatedRules << "\n";
        std::cout << "Alarmes noves creades: " << createdAlarms << "\n";
        std::cout << "Alarmes repetides controlades: " << repeatedAlarms << "\n";

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "[Database] ERROR SQL en runFase5M().\n";
        std::cerr << "Missatge: " << e.what() << "\n";
        std::cerr << "Codi error: " << e.getErrorCode() << "\n";
        std::cerr << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::resetFase5NData()
{
    try {
        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        // Tanquem lògicament alarmes obertes anteriors perquè no bloquegin la prova 5N.
        stmt->executeUpdate(
            "UPDATE alarm_events ae "
            "JOIN alarm_rules ar ON ar.id = ae.rule_id "
            "SET ae.status = 'CLOSED', "
            "    ae.event_state = 'RETURNED', "
            "    ae.closed_by = 'fase_5N_reset', "
            "    ae.closed_time_utc = CURRENT_TIMESTAMP(3), "
            "    ae.comment = 'Reset logic previ a Fase 5N' "
            "WHERE ae.status IN ('ACTIVE', 'ACKNOWLEDGED')"
        );

        stmt->executeUpdate(
            "DELETE ae FROM alarm_events ae "
            "JOIN sensor_readings sr ON sr.reading_id = ae.reading_id "
            "WHERE sr.source = 'cpp_fita_5N'"
        );

        stmt->executeUpdate(
            "DELETE FROM alarm_pending_conditions "
            "WHERE pending_source = 'cpp_fita_5N'"
        );

        stmt->executeUpdate(
            "DELETE FROM sensor_readings "
            "WHERE source = 'cpp_fita_5N'"
        );

        std::cout << "[5N] Dades de prova netejades.\n";
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR resetFase5NData(): " << e.what() << "\n";
        return false;
    }
}

long long Database::insertReadingFase5N(const std::string& sensorCode, double value, int secondsAgo)
{
    try {
        if (secondsAgo < 0) secondsAgo = 0;

        std::string sql =
            "INSERT INTO sensor_readings "
            "    (sensor_id, reading_time, reading_value, source) "
            "SELECT "
            "    sensor_id, "
            "    DATE_SUB(NOW(), INTERVAL " + std::to_string(secondsAgo) + " SECOND), "
            "    ?, "
            "    'cpp_fita_5N' "
            "FROM sensors "
            "WHERE sensor_code = ?";

        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(sql));
        pstmt->setDouble(1, value);
        pstmt->setString(2, sensorCode);

        int rows = pstmt->executeUpdate();

        if (rows == 0) {
            std::cerr << "[5N] No s'ha trobat el sensor: " << sensorCode << "\n";
            return -1;
        }

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));

        long long readingId = -1;
        if (rs->next()) {
            readingId = rs->getInt64("id");
        }

        std::cout << "[5N] Lectura inserida: "
                  << sensorCode << " = " << value
                  << " | reading_id=" << readingId
                  << " | source=cpp_fita_5N\n";

        return readingId;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR insertReadingFase5N(): " << e.what() << "\n";
        return -1;
    }
}

bool Database::forcePendingAgeFase5N(
    const std::string& sensorCode,
    const std::string& ruleCode,
    const std::string& alarmDirection,
    int secondsAgo
)
{
    try {
        if (secondsAgo < 0) secondsAgo = 0;

        std::string sql =
            "UPDATE alarm_pending_conditions "
            "SET first_abnormal_at = DATE_SUB(NOW(), INTERVAL " + std::to_string(secondsAgo) + " SECOND) "
            "WHERE sensor_code = ? "
            "  AND rule_code = ? "
            "  AND alarm_direction = ? "
            "  AND pending_source = 'cpp_fita_5N'";

        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(sql));
        pstmt->setString(1, sensorCode);
        pstmt->setString(2, ruleCode);
        pstmt->setString(3, alarmDirection);

        int rows = pstmt->executeUpdate();

        std::cout << "[5N] Envelliment de condicio pendent: "
                  << rows << " fila/es actualitzades.\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR forcePendingAgeFase5N(): " << e.what() << "\n";
        return false;
    }
}

bool Database::forceNormalSinceAgeFase5N(
    const std::string& ruleCode,
    const std::string& alarmDirection,
    int secondsAgo
)
{
    try {
        if (secondsAgo < 0) secondsAgo = 0;

        std::string sql =
            "UPDATE alarm_events ae "
            "JOIN alarm_rules ar ON ar.id = ae.rule_id "
            "SET ae.normal_since_at = DATE_SUB(NOW(), INTERVAL " + std::to_string(secondsAgo) + " SECOND) "
            "WHERE ar.rule_code = ? "
            "  AND ae.alarm_direction = ? "
            "  AND ae.status IN ('ACTIVE', 'ACKNOWLEDGED') "
            "  AND ae.normal_since_at IS NOT NULL";

        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(sql));
        pstmt->setString(1, ruleCode);
        pstmt->setString(2, alarmDirection);

        int rows = pstmt->executeUpdate();

        std::cout << "[5N] Envelliment de retorn a normal: "
                  << rows << " fila/es actualitzades.\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR forceNormalSinceAgeFase5N(): " << e.what() << "\n";
        return false;
    }
}

bool Database::runFase5N()
{
    std::cout << "============================================\n";
    std::cout << "FASE 5N - Histeresi, retard d'activacio i retorn a normal\n";
    std::cout << "============================================\n\n";

    if (!resetFase5NData()) {
        return false;
    }

    // CAS 1: primera lectura fora de rang.
    // Ha de crear condicio pendent, pero encara NO alarma.
    std::cout << "\n=== 5N.1 - Primera desviacio fora de rang ===\n";
    insertReadingFase5N("TEMP_COLD_01", 10.5);
    evaluateAlarmRulesFase5N();

    // Simulem que han passat mes de 60 segons.
    forcePendingAgeFase5N("TEMP_COLD_01", "RULE_TEMP_COLD_01", "HIGH", 65);

    // CAS 2: segona lectura fora de rang.
    // Ara el retard ja esta complert i s'ha de crear alarma ACTIVE.
    std::cout << "\n=== 5N.2 - Desviacio persistent: crea alarma ACTIVE ===\n";
    insertReadingFase5N("TEMP_COLD_01", 10.6);
    evaluateAlarmRulesFase5N();

    // CAS 3: valor dins del limit formal, pero encara dins banda d'histeresi.
    // Limit alt = 8.0, histeresi = 0.5. Per tant, retorn real nomes si <= 7.5.
    std::cout << "\n=== 5N.3 - Valor dins banda d'histeresi: alarma continua activa ===\n";
    insertReadingFase5N("TEMP_COLD_01", 7.8);
    evaluateAlarmRulesFase5N();

    // CAS 4: retorn clarament normal.
    // Ha d'iniciar normal_since_at.
    std::cout << "\n=== 5N.4 - Retorn a normal iniciat ===\n";
    insertReadingFase5N("TEMP_COLD_01", 7.3);
    evaluateAlarmRulesFase5N();

    // Simulem que el retorn normal s'ha mantingut mes de 60 segons.
    forceNormalSinceAgeFase5N("RULE_TEMP_COLD_01", "HIGH", 65);

    // CAS 5: continua normal. Ara s'ha de confirmar RETURNED.
    std::cout << "\n=== 5N.5 - Retorn a normal confirmat ===\n";
    insertReadingFase5N("TEMP_COLD_01", 7.2);
    evaluateAlarmRulesFase5N();

    printAlarmsFase5N();

    std::cout << "\nFase 5N completada correctament.\n";
    return true;
}

bool Database::evaluateAlarmRulesFase5N()
{
    try {
        std::string sql =
            "SELECT "
            "    ar.id AS rule_id, "
            "    ar.rule_code, "
            "    ar.sensor_code, "
            "    ar.alarm_low, "
            "    ar.alarm_high, "
            "    ar.hysteresis_value, "
            "    ar.activation_delay_seconds, "
            "    ar.return_normal_delay_seconds, "
            "    s.sensor_id, "
            "    sr.reading_id, "
            "    sr.reading_value "
            "FROM alarm_rules ar "
            "JOIN sensors s ON s.sensor_code = ar.sensor_code "
            "JOIN ( "
            "    SELECT sensor_id, MAX(reading_id) AS max_reading_id "
            "    FROM sensor_readings "
            "    WHERE source = 'cpp_fita_5N' "
            "    GROUP BY sensor_id "
            ") last_sr ON last_sr.sensor_id = s.sensor_id "
            "JOIN sensor_readings sr ON sr.reading_id = last_sr.max_reading_id "
            "WHERE ar.enabled = 1 "
            "ORDER BY ar.rule_code";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(sql));

        auto processDirection =
            [&](int ruleId,
                const std::string& ruleCode,
                const std::string& sensorCode,
                long long readingId,
                double value,
                double limit,
                double hysteresis,
                int activationDelay,
                int returnDelay,
                bool isHigh) -> bool
        {
            std::string direction = isHigh ? "HIGH" : "LOW";

            bool abnormal = isHigh ? (value > limit) : (value < limit);
            bool normalEnough = isHigh
                ? (value <= limit - hysteresis)
                : (value >= limit + hysteresis);

            // 1) Mirem si ja hi ha una alarma oberta per aquesta regla i direccio.
            {
                std::string q =
                    "SELECT "
                    "    alarm_event_id, "
                    "    IF(normal_since_at IS NULL, 0, 1) AS has_normal_since, "
                    "    IFNULL(TIMESTAMPDIFF(SECOND, normal_since_at, NOW()), 0) AS normal_age "
                    "FROM alarm_events "
                    "WHERE rule_id = ? "
                    "  AND alarm_direction = ? "
                    "  AND status IN ('ACTIVE', 'ACKNOWLEDGED') "
                    "ORDER BY alarm_event_id DESC "
                    "LIMIT 1";

                std::unique_ptr<sql::PreparedStatement> ps(connection_->prepareStatement(q));
                ps->setInt(1, ruleId);
                ps->setString(2, direction);

                std::unique_ptr<sql::ResultSet> openAlarm(ps->executeQuery());

                if (openAlarm->next()) {
                    long long alarmEventId = openAlarm->getInt64("alarm_event_id");
                    int hasNormalSince = openAlarm->getInt("has_normal_since");
                    int normalAge = openAlarm->getInt("normal_age");

                    if (abnormal) {
                        std::string u =
                            "UPDATE alarm_events "
                            "SET last_alarm_value = ?, "
                            "    last_seen_at = NOW(), "
                            "    normal_since_at = NULL, "
                            "    repeat_count = repeat_count + 1, "
                            "    comment = 'Fase 5N: alarma repetida, encara fora de rang' "
                            "WHERE alarm_event_id = ?";

                        std::unique_ptr<sql::PreparedStatement> up(connection_->prepareStatement(u));
                        up->setDouble(1, value);
                        up->setInt64(2, alarmEventId);
                        up->executeUpdate();

                        std::cout << "ALARMA REPETIDA CONTROLADA: "
                                  << "alarm_event_id=" << alarmEventId
                                  << " sensor=" << sensorCode
                                  << " regla=" << ruleCode
                                  << " direccio=" << direction
                                  << " valor=" << value << "\n";

                        return true;
                    }

                    if (normalEnough) {
                        if (hasNormalSince == 0) {
                            std::string u =
                                "UPDATE alarm_events "
                                "SET normal_since_at = NOW(), "
                                "    return_normal_value = ?, "
                                "    last_seen_at = NOW(), "
                                "    comment = 'Fase 5N: retorn a normal iniciat' "
                                "WHERE alarm_event_id = ?";

                            std::unique_ptr<sql::PreparedStatement> up(connection_->prepareStatement(u));
                            up->setDouble(1, value);
                            up->setInt64(2, alarmEventId);
                            up->executeUpdate();

                            std::cout << "RETORN A NORMAL INICIAT: "
                                      << "alarm_event_id=" << alarmEventId
                                      << " sensor=" << sensorCode
                                      << " valor=" << value << "\n";

                            return true;
                        }

                        if (normalAge >= returnDelay) {
                            std::string u =
                                "UPDATE alarm_events "
                                "SET status = 'RETURNED', "
                                "    event_state = 'RETURNED', "
                                "    returned_to_normal_at = NOW(), "
                                "    return_normal_value = ?, "
                                "    closed_time_utc = CURRENT_TIMESTAMP(3), "
                                "    closed_by = 'system_5N', "
                                "    comment = 'Fase 5N: retorn a normal confirmat' "
                                "WHERE alarm_event_id = ?";

                            std::unique_ptr<sql::PreparedStatement> up(connection_->prepareStatement(u));
                            up->setDouble(1, value);
                            up->setInt64(2, alarmEventId);
                            up->executeUpdate();

                            std::cout << "RETURN_TO_NORMAL: "
                                      << "alarm_event_id=" << alarmEventId
                                      << " sensor=" << sensorCode
                                      << " regla=" << ruleCode
                                      << " valor=" << value << "\n";

                            return true;
                        }

                        std::cout << "[5N] Retorn normal encara no confirmat: "
                                  << sensorCode
                                  << " normal_age=" << normalAge
                                  << "s / requerit=" << returnDelay << "s\n";

                        return true;
                    }

                    // Ni anormal ni prou normal: zona d'histeresi.
                    std::string u =
                        "UPDATE alarm_events "
                        "SET normal_since_at = NULL, "
                        "    last_seen_at = NOW(), "
                        "    comment = 'Fase 5N: dins banda d_histeresi, alarma mantinguda' "
                        "WHERE alarm_event_id = ?";

                    std::unique_ptr<sql::PreparedStatement> up(connection_->prepareStatement(u));
                    up->setInt64(1, alarmEventId);
                    up->executeUpdate();

                    std::cout << "[5N] Dins banda d'histeresi: "
                              << sensorCode
                              << " valor=" << value
                              << ". L'alarma continua activa.\n";

                    return true;
                }
            }

            // 2) Si no hi ha alarma oberta, gestionem condicio pendent.
            if (abnormal) {
                std::string q =
                    "SELECT "
                    "    pending_id, "
                    "    TIMESTAMPDIFF(SECOND, first_abnormal_at, NOW()) AS age_seconds "
                    "FROM alarm_pending_conditions "
                    "WHERE sensor_code = ? "
                    "  AND rule_code = ? "
                    "  AND alarm_direction = ? "
                    "  AND pending_source = 'cpp_fita_5N' "
                    "LIMIT 1";

                std::unique_ptr<sql::PreparedStatement> ps(connection_->prepareStatement(q));
                ps->setString(1, sensorCode);
                ps->setString(2, ruleCode);
                ps->setString(3, direction);

                std::unique_ptr<sql::ResultSet> pending(ps->executeQuery());

                if (!pending->next()) {
                    std::string ins =
                        "INSERT INTO alarm_pending_conditions "
                        "    (sensor_code, rule_code, alarm_direction, "
                        "     first_abnormal_at, last_abnormal_at, last_reading_value, pending_source) "
                        "VALUES (?, ?, ?, NOW(), NOW(), ?, 'cpp_fita_5N')";

                    std::unique_ptr<sql::PreparedStatement> pi(connection_->prepareStatement(ins));
                    pi->setString(1, sensorCode);
                    pi->setString(2, ruleCode);
                    pi->setString(3, direction);
                    pi->setDouble(4, value);
                    pi->executeUpdate();

                    std::cout << "[5N] Condicio pendent creada: "
                              << sensorCode
                              << " regla=" << ruleCode
                              << " direccio=" << direction
                              << " valor=" << value
                              << ". Encara no es crea alarma.\n";

                    return true;
                }

                long long pendingId = pending->getInt64("pending_id");
                int ageSeconds = pending->getInt("age_seconds");

                {
                    std::string u =
                        "UPDATE alarm_pending_conditions "
                        "SET last_abnormal_at = NOW(), "
                        "    last_reading_value = ? "
                        "WHERE pending_id = ?";

                    std::unique_ptr<sql::PreparedStatement> up(connection_->prepareStatement(u));
                    up->setDouble(1, value);
                    up->setInt64(2, pendingId);
                    up->executeUpdate();
                }

                if (ageSeconds >= activationDelay) {
                    std::string ins =
                        "INSERT INTO alarm_events "
                        "    (rule_id, reading_id, activation_value, alarm_value, "
                        "     alarm_direction, last_alarm_value, last_seen_at, comment) "
                        "VALUES (?, ?, ?, ?, ?, ?, NOW(), "
                        "        'Fase 5N: alarma creada despres de retard d_activacio')";

                    std::unique_ptr<sql::PreparedStatement> ai(connection_->prepareStatement(ins));
                    ai->setInt(1, ruleId);
                    ai->setInt64(2, readingId);
                    ai->setDouble(3, value);
                    ai->setDouble(4, value);
                    ai->setString(5, direction);
                    ai->setDouble(6, value);
                    ai->executeUpdate();

                    std::string del =
                        "DELETE FROM alarm_pending_conditions "
                        "WHERE pending_id = ?";

                    std::unique_ptr<sql::PreparedStatement> dp(connection_->prepareStatement(del));
                    dp->setInt64(1, pendingId);
                    dp->executeUpdate();

                    std::cout << "ALARMA ACTIVE creada: "
                              << sensorCode
                              << " regla=" << ruleCode
                              << " direccio=" << direction
                              << " valor=" << value
                              << " retard=" << ageSeconds << "s\n";

                    return true;
                }

                std::cout << "[5N] Condicio pendent encara insuficient: "
                          << sensorCode
                          << " age=" << ageSeconds
                          << "s / requerit=" << activationDelay << "s\n";

                return true;
            }

            // 3) Si no hi ha alarma oberta i la lectura ja no es anormal,
            // eliminem qualsevol pendent residual.
            {
                std::string del =
                    "DELETE FROM alarm_pending_conditions "
                    "WHERE sensor_code = ? "
                    "  AND rule_code = ? "
                    "  AND alarm_direction = ? "
                    "  AND pending_source = 'cpp_fita_5N'";

                std::unique_ptr<sql::PreparedStatement> dp(connection_->prepareStatement(del));
                dp->setString(1, sensorCode);
                dp->setString(2, ruleCode);
                dp->setString(3, direction);

                int rows = dp->executeUpdate();

                if (rows > 0) {
                    std::cout << "[5N] Condicio pendent cancelada per retorn abans d'activacio: "
                              << sensorCode
                              << " regla=" << ruleCode
                              << " direccio=" << direction << "\n";
                }
            }

            return true;
        };

        int processed = 0;

        while (rs->next()) {
            int ruleId = rs->getInt("rule_id");
            std::string ruleCode = rs->getString("rule_code");
            std::string sensorCode = rs->getString("sensor_code");

            long long readingId = rs->getInt64("reading_id");
            double value = rs->getDouble("reading_value");

            double alarmLow = rs->getDouble("alarm_low");
            double alarmHigh = rs->getDouble("alarm_high");
            double hysteresis = rs->getDouble("hysteresis_value");
            int activationDelay = rs->getInt("activation_delay_seconds");
            int returnDelay = rs->getInt("return_normal_delay_seconds");

            // HIGH
            processDirection(
                ruleId,
                ruleCode,
                sensorCode,
                readingId,
                value,
                alarmHigh,
                hysteresis,
                activationDelay,
                returnDelay,
                true
            );

            // LOW
            processDirection(
                ruleId,
                ruleCode,
                sensorCode,
                readingId,
                value,
                alarmLow,
                hysteresis,
                activationDelay,
                returnDelay,
                false
            );

            processed++;
        }

        std::cout << "[5N] Regles avaluades: " << processed << "\n";
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR evaluateAlarmRulesFase5N(): " << e.what() << "\n";
        return false;
    }
}

bool Database::printAlarmsFase5N()
{
    try {
        std::cout << "\n=== ALARMES FASE 5N ===\n";

        std::string sql =
            "SELECT "
            "    ae.alarm_event_id, "
            "    ar.rule_code, "
            "    ar.sensor_code, "
            "    ae.status, "
            "    ae.alarm_direction, "
            "    ae.alarm_value, "
            "    ae.last_alarm_value, "
            "    ae.repeat_count, "
            "    ae.last_seen_at, "
            "    ae.normal_since_at, "
            "    ae.returned_to_normal_at, "
            "    ae.return_normal_value "
            "FROM alarm_events ae "
            "JOIN alarm_rules ar ON ar.id = ae.rule_id "
            "LEFT JOIN sensor_readings sr ON sr.reading_id = ae.reading_id "
            "WHERE sr.source = 'cpp_fita_5N' "
            "ORDER BY ae.alarm_event_id";

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(sql));

        int count = 0;

        while (rs->next()) {
            count++;

            std::cout << "alarm_event_id=" << rs->getInt64("alarm_event_id")
                      << " | sensor=" << rs->getString("sensor_code")
                      << " | regla=" << rs->getString("rule_code")
                      << " | status=" << rs->getString("status")
                      << " | direccio=" << rs->getString("alarm_direction")
                      << " | alarm_value=" << rs->getDouble("alarm_value")
                      << " | last_alarm_value=" << rs->getDouble("last_alarm_value")
                      << " | repeat_count=" << rs->getInt("repeat_count")
                      << " | return_normal_value=" << rs->getDouble("return_normal_value")
                      << "\n";
        }

        std::cout << "Alarmes Fase 5N mostrades: " << count << "\n";

        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[5N] ERROR printAlarmsFase5N(): " << e.what() << "\n";
        return false;
    }
}

bool Database::printAlarmTraceabilityReportFase5O(int limit)
{
    if (connection_ == nullptr)
    {
        std::cout << "[5O] ERROR: No hi ha connexio activa a MySQL.\n";
        return false;
    }

    try
    {
        std::cout << "\n============================================\n";
        std::cout << "FASE 5O - Informe de tracabilitat d'alarmes\n";
        std::cout << "============================================\n\n";

        std::string query =
            "SELECT "
            "  ae.alarm_event_id AS alarm_event_id, "
            "  s.sensor_code, "
            "  COALESCE(z.zone_code, 'NULL') AS zone_code, "
            "  ar.rule_code, "
            "  ar.priority, "
            "  ae.status, "
            "  CASE "
            "    WHEN ar.alarm_high IS NOT NULL AND ae.alarm_value > ar.alarm_high THEN 'HIGH' "
            "    WHEN ar.alarm_low IS NOT NULL AND ae.alarm_value < ar.alarm_low THEN 'LOW' "
            "    ELSE 'UNKNOWN' "
            "  END AS direction, "
            "  ae.alarm_value, "
            "  ae.last_alarm_value, "
            "  ae.return_normal_value, "
            "  ar.alarm_low, "
            "  ar.alarm_high, "
            "  ae.triggered_at, "
            "  NULL AS acknowledged_at, "
            "  NULL AS acknowledged_by, "
            "  NULL AS closed_at, "
            "  NULL AS closed_by, "
            "  sr.reading_id, "
            "  sr.reading_value, "
            "  sr.source "
            "FROM alarm_events ae "
            "JOIN alarm_rules ar ON ae.rule_id = ar.id "
            "JOIN sensors s ON ar.sensor_code = s.sensor_code "
            "LEFT JOIN storage_zones z ON s.zone_id = z.zone_id "
            "LEFT JOIN sensor_readings sr ON ae.reading_id = sr.reading_id "
            "ORDER BY ae.alarm_event_id DESC "
            "LIMIT " + std::to_string(limit);

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        int count = 0;

        while (res->next())
        {
            ++count;

            std::cout << "--------------------------------------------\n";
            std::cout << "Alarma ID:      " << res->getInt("alarm_event_id") << "\n";
            std::cout << "Sensor:         " << res->getString("sensor_code") << "\n";
            std::cout << "Zona:           " << res->getString("zone_code") << "\n";
            std::cout << "Regla:          " << res->getString("rule_code") << "\n";
            std::cout << "Prioritat:      " << res->getString("priority") << "\n";
            std::cout << "Estat:          " << res->getString("status") << "\n";
            std::cout << "Direccio:       " << res->getString("direction") << "\n";

            std::cout << "Valor alarma:   " << res->getDouble("alarm_value") << "\n";

            if (!res->isNull("last_alarm_value"))
                std::cout << "Ultim valor:    " << res->getDouble("last_alarm_value") << "\n";
            else
                std::cout << "Ultim valor:    NULL\n";

            if (!res->isNull("return_normal_value"))
                std::cout << "Retorn normal:  " << res->getDouble("return_normal_value") << "\n";
            else
                std::cout << "Retorn normal:  NULL\n";

            if (!res->isNull("alarm_low"))
                std::cout << "Limit baix:     " << res->getDouble("alarm_low") << "\n";
            else
                std::cout << "Limit baix:     NULL\n";

            if (!res->isNull("alarm_high"))
                std::cout << "Limit alt:      " << res->getDouble("alarm_high") << "\n";
            else
                std::cout << "Limit alt:      NULL\n";

            std::cout << "Activada:       " << res->getString("triggered_at") << "\n";

            if (!res->isNull("acknowledged_at"))
                std::cout << "ACK at:         " << res->getString("acknowledged_at") << "\n";
            else
                std::cout << "ACK at:         NULL\n";

            if (!res->isNull("acknowledged_by"))
                std::cout << "ACK by:         " << res->getString("acknowledged_by") << "\n";
            else
                std::cout << "ACK by:         NULL\n";

            if (!res->isNull("closed_at"))
                std::cout << "Closed at:      " << res->getString("closed_at") << "\n";
            else
                std::cout << "Closed at:      NULL\n";

            if (!res->isNull("closed_by"))
                std::cout << "Closed by:      " << res->getString("closed_by") << "\n";
            else
                std::cout << "Closed by:      NULL\n";

            if (!res->isNull("reading_id"))
            {
                std::cout << "Lectura origen: " << res->getInt("reading_id")
                          << " | valor=" << res->getDouble("reading_value")
                          << " | source=" << res->getString("source") << "\n";
            }
            else
            {
                std::cout << "Lectura origen: NULL\n";
            }
        }

        std::cout << "--------------------------------------------\n";
        std::cout << "[5O] Alarmes incloses a l'informe: " << count << "\n";

        if (count == 0)
        {
            std::cout << "[5O] Avis: no s'han trobat alarmes a alarm_events.\n";
        }

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cout << "[5O] ERROR SQL generant informe de tracabilitat.\n";
        std::cout << "Missatge: " << e.what() << "\n";
        std::cout << "Codi error: " << e.getErrorCode() << "\n";
        std::cout << "SQLState: " << e.getSQLState() << "\n";
        return false;
    }
}

bool Database::exportAlarmTraceabilityCsv(const std::string& outputPath)
{
    try
    {
        _mkdir("reports");

        std::ofstream file(outputPath, std::ios::out | std::ios::trunc);

        if (!file.is_open())
        {
            std::cout << "[5P-b] ERROR: no s'ha pogut crear el fitxer CSV: "
                      << outputPath << std::endl;
            return false;
        }

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        std::string query = R"SQL(
            SELECT
                ae.alarm_event_id,
                s.sensor_code,
                sz.zone_code,
                ar.rule_code,
                ar.priority,
                ae.status,

                CASE
                    WHEN ar.alarm_low IS NOT NULL
                         AND ae.alarm_value < ar.alarm_low
                        THEN 'LOW'
                    WHEN ar.alarm_high IS NOT NULL
                         AND ae.alarm_value > ar.alarm_high
                        THEN 'HIGH'
                    ELSE 'UNKNOWN'
                END AS direction,

                ae.alarm_value,
                ae.last_alarm_value,
                ae.return_normal_value,
                ar.alarm_low,
                ar.alarm_high,
                ae.triggered_at,

                aud.acknowledged_at,
                aud.acknowledged_by,
                aud.closed_at,
                aud.closed_by,

                sr.reading_id AS source_reading_id,
                sr.reading_value AS source_value,
                sr.source AS source

            FROM alarm_events ae

            INNER JOIN alarm_rules ar
                ON ae.rule_id = ar.id

            INNER JOIN sensors s
                ON ar.sensor_code = s.sensor_code

            LEFT JOIN storage_zones sz
                ON s.zone_id = sz.zone_id

            LEFT JOIN sensor_readings sr
                ON ae.reading_id = sr.reading_id

            LEFT JOIN
            (
                SELECT
                    record_id AS alarm_event_id,

                    MIN(CASE
                        WHEN action = 'ACK_ALARM'
                        THEN changed_at
                    END) AS acknowledged_at,

                    SUBSTRING_INDEX(
                        GROUP_CONCAT(
                            CASE
                                WHEN action = 'ACK_ALARM'
                                THEN changed_by
                            END
                            ORDER BY changed_at ASC
                            SEPARATOR ','
                        ),
                        ',',
                        1
                    ) AS acknowledged_by,

                    MIN(CASE
                        WHEN action = 'CLOSE_ALARM'
                        THEN changed_at
                    END) AS closed_at,

                    SUBSTRING_INDEX(
                        GROUP_CONCAT(
                            CASE
                                WHEN action = 'CLOSE_ALARM'
                                THEN changed_by
                            END
                            ORDER BY changed_at ASC
                            SEPARATOR ','
                        ),
                        ',',
                        1
                    ) AS closed_by

                FROM audit_trail
                WHERE table_name IN ('alarm_events', 'alarm_event')
                  AND action IN ('ACK_ALARM', 'CLOSE_ALARM')
                GROUP BY record_id
            ) aud
                ON aud.alarm_event_id = ae.alarm_event_id

            ORDER BY ae.alarm_event_id DESC
        )SQL";

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        std::vector<std::string> columns =
        {
            "alarm_event_id",
            "sensor_code",
            "zone_code",
            "rule_code",
            "priority",
            "status",
            "direction",
            "alarm_value",
            "last_alarm_value",
            "return_normal_value",
            "alarm_low",
            "alarm_high",
            "triggered_at",
            "acknowledged_at",
            "acknowledged_by",
            "closed_at",
            "closed_by",
            "source_reading_id",
            "source_value",
            "source"
        };

        for (size_t i = 0; i < columns.size(); ++i)
        {
            if (i > 0)
            {
                file << ",";
            }

            file << columns[i];
        }

        file << "\n";

        int rowCount = 0;

        while (res->next())
        {
            for (size_t i = 0; i < columns.size(); ++i)
            {
                if (i > 0)
                {
                    file << ",";
                }

                file << csvField(res.get(), columns[i]);
            }

            file << "\n";
            ++rowCount;
        }

        file.close();

        std::cout << "[5P-b] CSV generat correctament: "
                  << outputPath << std::endl;

        std::cout << "[5P-b] Files exportades: "
                  << rowCount << std::endl;

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cout << "[5P-b] ERROR SQL exportant CSV." << std::endl;
        std::cout << "Missatge: " << e.what() << std::endl;
        std::cout << "Codi error: " << e.getErrorCode() << std::endl;
        std::cout << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    }
    catch (std::exception& e)
    {
        std::cout << "[5P-b] ERROR general exportant CSV." << std::endl;
        std::cout << "Missatge: " << e.what() << std::endl;
        return false;
    }
}

bool Database::exportAlarmTraceabilityHtml(const std::string& outputPath)
{
    try
    {
        _mkdir("reports");

        std::unique_ptr<sql::Statement> stmt(connection_->createStatement());

        std::string query = R"SQL(
            SELECT
                ae.alarm_event_id,
                s.sensor_code,
                sz.zone_code,
                ar.rule_code,
                ar.priority,
                ae.status,

                CASE
                    WHEN ar.alarm_low IS NOT NULL
                         AND ae.alarm_value < ar.alarm_low
                        THEN 'LOW'
                    WHEN ar.alarm_high IS NOT NULL
                         AND ae.alarm_value > ar.alarm_high
                        THEN 'HIGH'
                    ELSE 'UNKNOWN'
                END AS direction,

                ae.alarm_value,
                ae.last_alarm_value,
                ae.return_normal_value,
                ar.alarm_low,
                ar.alarm_high,
                ae.triggered_at,

                aud.acknowledged_at,
                aud.acknowledged_by,
                aud.closed_at,
                aud.closed_by,

                sr.reading_id AS source_reading_id,
                sr.reading_value AS source_value,
                sr.source AS source

            FROM alarm_events ae

            INNER JOIN alarm_rules ar
                ON ae.rule_id = ar.id

            INNER JOIN sensors s
                ON ar.sensor_code = s.sensor_code

            LEFT JOIN storage_zones sz
                ON s.zone_id = sz.zone_id

            LEFT JOIN sensor_readings sr
                ON ae.reading_id = sr.reading_id

            LEFT JOIN
            (
                SELECT
                    record_id AS alarm_event_id,

                    MIN(CASE
                        WHEN action = 'ACK_ALARM'
                        THEN changed_at
                    END) AS acknowledged_at,

                    SUBSTRING_INDEX(
                        GROUP_CONCAT(
                            CASE
                                WHEN action = 'ACK_ALARM'
                                THEN changed_by
                            END
                            ORDER BY changed_at ASC
                            SEPARATOR ','
                        ),
                        ',',
                        1
                    ) AS acknowledged_by,

                    MIN(CASE
                        WHEN action = 'CLOSE_ALARM'
                        THEN changed_at
                    END) AS closed_at,

                    SUBSTRING_INDEX(
                        GROUP_CONCAT(
                            CASE
                                WHEN action = 'CLOSE_ALARM'
                                THEN changed_by
                            END
                            ORDER BY changed_at ASC
                            SEPARATOR ','
                        ),
                        ',',
                        1
                    ) AS closed_by

                FROM audit_trail
                WHERE table_name IN ('alarm_events', 'alarm_event')
                  AND action IN ('ACK_ALARM', 'CLOSE_ALARM')
                GROUP BY record_id
            ) aud
                ON aud.alarm_event_id = ae.alarm_event_id

            ORDER BY ae.alarm_event_id DESC
        )SQL";

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

        std::vector<std::string> columns =
        {
            "alarm_event_id",
            "sensor_code",
            "zone_code",
            "rule_code",
            "priority",
            "status",
            "direction",
            "alarm_value",
            "last_alarm_value",
            "return_normal_value",
            "alarm_low",
            "alarm_high",
            "triggered_at",
            "acknowledged_at",
            "acknowledged_by",
            "closed_at",
            "closed_by",
            "source_reading_id",
            "source_value",
            "source"
        };

        std::vector<std::string> labels =
        {
            "Alarm ID",
            "Sensor",
            "Zona",
            "Regla",
            "Prioritat",
            "Estat",
            "Direccio",
            "Valor alarma",
            "Ultim valor",
            "Retorn normal",
            "Limit baix",
            "Limit alt",
            "Activada",
            "ACK at",
            "ACK by",
            "Closed at",
            "Closed by",
            "Lectura origen",
            "Valor lectura",
            "Source"
        };

        std::vector<std::vector<std::string>> rows;
        std::map<std::string, int> countByStatus;
        std::map<std::string, int> countByPriority;

        int acknowledgedCount = 0;
        int closedCount = 0;

        while (res->next())
        {
            std::vector<std::string> row;

            for (const auto& column : columns)
            {
                row.push_back(plainField(res.get(), column));
            }

            countByPriority[row[4]]++;
            countByStatus[row[5]]++;

            if (row[13] != "NULL")
            {
                acknowledgedCount++;
            }

            if (row[15] != "NULL")
            {
                closedCount++;
            }

            rows.push_back(row);
        }

        std::ofstream file(outputPath, std::ios::out | std::ios::trunc);

        if (!file.is_open())
        {
            std::cout << "[5Q] ERROR: no s'ha pogut crear l'informe HTML: "
                      << outputPath << std::endl;
            return false;
        }

        file << R"HTML(<!DOCTYPE html>
<html lang="ca">
<head>
<meta charset="UTF-8">
<title>Fase 5Q - Informe de tracabilitat d'alarmes</title>
<style>
body {
    font-family: Arial, sans-serif;
    margin: 24px;
    background: #f7f7f7;
    color: #222;
}
h1, h2 {
    margin-bottom: 8px;
}
.card {
    background: white;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 16px;
    margin-bottom: 20px;
}
table {
    border-collapse: collapse;
    width: 100%;
    background: white;
    font-size: 13px;
}
th, td {
    border: 1px solid #ccc;
    padding: 6px 8px;
    text-align: left;
    vertical-align: top;
}
th {
    background: #eaeaea;
}
.summary-table {
    width: auto;
    min-width: 320px;
}
.footer {
    margin-top: 24px;
    font-size: 12px;
    color: #555;
}
</style>
</head>
<body>
)HTML";

        file << "<h1>Fase 5Q - Informe de tracabilitat d'alarmes</h1>\n";

        file << "<div class=\"card\">\n";
        file << "<h2>Resum general</h2>\n";
        file << "<p><strong>Total d'alarmes exportades:</strong> "
             << rows.size() << "</p>\n";
        file << "<p><strong>Alarmes amb ACK reconstruït:</strong> "
             << acknowledgedCount << "</p>\n";
        file << "<p><strong>Alarmes amb CLOSE reconstruït:</strong> "
             << closedCount << "</p>\n";
        file << "</div>\n";

        auto writeCountTable =
            [&file](const std::string& title, const std::map<std::string, int>& counts)
        {
            file << "<div class=\"card\">\n";
            file << "<h2>" << htmlEscape(title) << "</h2>\n";
            file << "<table class=\"summary-table\">\n";
            file << "<tr><th>Categoria</th><th>Quantitat</th></tr>\n";

            for (const auto& item : counts)
            {
                file << "<tr><td>"
                     << htmlEscape(item.first)
                     << "</td><td>"
                     << item.second
                     << "</td></tr>\n";
            }

            file << "</table>\n";
            file << "</div>\n";
        };

        writeCountTable("Alarmes per prioritat", countByPriority);
        writeCountTable("Alarmes per estat", countByStatus);

        file << "<div class=\"card\">\n";
        file << "<h2>Detall de tracabilitat</h2>\n";
        file << "<table>\n";
        file << "<tr>";

        for (const auto& label : labels)
        {
            file << "<th>" << htmlEscape(label) << "</th>";
        }

        file << "</tr>\n";

        for (const auto& row : rows)
        {
            file << "<tr>";

            for (const auto& value : row)
            {
                file << "<td>" << htmlEscape(value) << "</td>";
            }

            file << "</tr>\n";
        }

        file << "</table>\n";
        file << "</div>\n";

        file << R"HTML(
<div class="footer">
Informe generat pel projecte FarmaciaInteligenteCpp_VS. Fase 5Q.
</div>
</body>
</html>
)HTML";

        file.close();

        std::cout << "[5Q] Informe HTML generat correctament: "
                  << outputPath << std::endl;

        std::cout << "[5Q] Alarmes incloses a l'informe: "
                  << rows.size() << std::endl;

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cout << "[5Q] ERROR SQL generant informe HTML." << std::endl;
        std::cout << "Missatge: " << e.what() << std::endl;
        std::cout << "Codi error: " << e.getErrorCode() << std::endl;
        std::cout << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    }
    catch (std::exception& e)
    {
        std::cout << "[5Q] ERROR general generant informe HTML." << std::endl;
        std::cout << "Missatge: " << e.what() << std::endl;
        return false;
    }
}

void Database::disconnect()
{
    if (connection_)
    {
        connection_->close();
        connection_.reset();
    }
}