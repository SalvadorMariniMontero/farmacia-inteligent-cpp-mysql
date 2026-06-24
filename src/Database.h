#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <memory>
#include <string>

class Database
{
public:
    Database(
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& schema
    );

    ~Database();

    // Connexio i proves basiques
    bool connect();
    void disconnect();
    bool testVersion();

    // Lectura de dades mestres
    bool listStorageZones();
    bool listSensors();
    bool printAlarmRules();

    // Fase 5J - Lectures simulades
    bool insertReadingBySensorCode(
        const std::string& sensorCode,
        double readingValue,
        const std::string& source
    );

    bool insertSimulatedReadings5J();
    bool printLatestSensorReadings5J(int limit = 12);

    // Fase 5K - Generacio d'alarmes ACTIVE
    bool insertOutOfRangeReadingsFase5K();
    bool evaluateReadingsAndCreateAlarmsFase5K();
    bool printAlarmsFase5K();

    // Fase 5L - ACK, CLOSE i audit trail
    bool acknowledgeActiveAlarmsFase5L(const std::string& user);
    bool closeAcknowledgedAlarmsFase5L(const std::string& user);
    bool printAlarmLifecycleFase5L();
    bool printAuditTrailFase5L();

    // Fase 5M - Control de duplicats
    int findOpenAlarmEventId(
        int ruleId,
        const std::string& alarmDirection
    );

    bool updateRepeatedAlarm(
        int alarmEventId,
        double currentValue
    );

    int createActiveAlarmEvent5M(
        int ruleId,
        int readingId,
        double alarmValue,
        const std::string& alarmDirection
    );

    bool runFase5M();

    // Fase 5N - Histeresi, retard i retorn a normal
    bool runFase5N();
    bool resetFase5NData();

    long long insertReadingFase5N(
        const std::string& sensorCode,
        double value,
        int secondsAgo = 0
    );

    bool evaluateAlarmRulesFase5N();
    bool printAlarmsFase5N();

    bool forcePendingAgeFase5N(
        const std::string& sensorCode,
        const std::string& ruleCode,
        const std::string& alarmDirection,
        int secondsAgo
    );

    bool forceNormalSinceAgeFase5N(
        const std::string& ruleCode,
        const std::string& alarmDirection,
        int secondsAgo
    );

    // Fases 5O, 5P-b i 5Q - Informes de tracabilitat
    bool printAlarmTraceabilityReportFase5O(int limit = 20);
    bool exportAlarmTraceabilityCsv(const std::string& outputPath);
    bool exportAlarmTraceabilityHtml(const std::string& outputPath);

private:
    sql::mysql::MySQL_Driver* driver_;
    std::unique_ptr<sql::Connection> connection_;

    std::string host_;
    std::string user_;
    std::string password_;
    std::string schema_;

    bool insertAuditTrailFase5L(
        const std::string& tableName,
        long long recordId,
        const std::string& action,
        const std::string& oldValue,
        const std::string& newValue,
        const std::string& changedBy
    );
};