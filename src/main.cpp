#include "Database.h"

#include <iostream>
#include <string>

int main()
{
    std::string password;

    std::cout << "Introdueix la contrasenya de MySQL: ";
    std::getline(std::cin, password);

    Database db(
        "tcp://127.0.0.1:3306",
        "farmacia_app",
        password,
        "farmacia_inteligente"
    );

    if (!db.connect())
    {
        std::cerr << "Connexio fallida amb MySQL.\n";
        return 1;
    }

    db.testVersion();

    std::cout << "\n============================================\n";
    std::cout << "FASE 5R - Revisio final de codi i informes\n";
    std::cout << "============================================\n";

    std::cout << "\n[5R] Generant informe de tracabilitat per consola...\n";

    if (!db.printAlarmTraceabilityReportFase5O(20))
    {
        std::cout << "\n[5R] Informe de tracabilitat per consola finalitzat amb errors.\n";
        return 1;
    }

    std::cout << "\n[5R] Informe de tracabilitat per consola generat correctament.\n";

    std::cout << "\n[5R] Generant CSV amb audit trail...\n";

    const std::string csvPath =
        "reports\\fase_5P_b_alarm_traceability_with_audit.csv";

    if (!db.exportAlarmTraceabilityCsv(csvPath))
    {
        std::cout << "\n[5R] Exportacio CSV fallida.\n";
        return 1;
    }

    std::cout << "\n[5R] Exportacio CSV completada correctament.\n";

    std::cout << "\n[5R] Generant informe HTML final...\n";

    const std::string htmlPath =
        "reports\\fase_5Q_alarm_traceability_report.html";

    if (!db.exportAlarmTraceabilityHtml(htmlPath))
    {
        std::cout << "\n[5R] Informe HTML fallit.\n";
        return 1;
    }

    std::cout << "\n[5R] Informe HTML completat correctament.\n";

    std::cout << "\n============================================\n";
    std::cout << "FASE 5R COMPLETADA CORRECTAMENT\n";
    std::cout << "============================================\n";

    return 0;
}