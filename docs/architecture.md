\# Arquitectura del prototip



Aquest document descriu l’arquitectura general del prototip \*\*Farmàcia Intel·ligent en C++ i MySQL\*\*.



El projecte és una demostració tècnica orientada a la gestió d’alarmes, audit trail i traçabilitat documental en un context farmacèutic simulat. No és un sistema GMP/GDP validat, sinó una base experimental d’enginyeria software.



\## 1. Visió general



El sistema està format per tres capes principals:



1\. Aplicació C++ de consola.

2\. Base de dades MySQL.

3\. Informes documentals en CSV i HTML.



La funció principal de l’aplicació és connectar amb MySQL, llegir la configuració de zones, sensors i regles d’alarma, inserir lectures simulades, avaluar desviacions i generar registres traçables.



\## 2. Capa C++



La capa C++ conté la lògica principal del prototip.



Fitxers principals:



\* `src/main.cpp`

\* `src/Database.h`

\* `src/Database.cpp`



Responsabilitats principals:



\* inicialitzar l’execució del programa;

\* demanar la contrasenya MySQL per consola;

\* establir connexió amb la base de dades;

\* executar consultes SQL;

\* inserir lectures simulades;

\* avaluar regles d’alarma;

\* generar alarmes;

\* registrar accions d’audit trail;

\* exportar informes CSV i HTML.



\## 3. Capa MySQL



La base de dades conté les taules principals del sistema.



Taules principals:



\* `storage\_zones`

\* `sensors`

\* `alarm\_rules`

\* `sensor\_readings`

\* `alarm\_events`

\* `audit\_trail`

\* `alarm\_pending\_conditions`



\### `storage\_zones`



Defineix les zones d’emmagatzematge del sistema, per exemple zona ambient, cambra freda o zona robotitzada.



\### `sensors`



Defineix els sensors associats a cada zona, amb el seu tipus, unitat i estat actiu.



\### `alarm\_rules`



Defineix les regles d’alarma associades als sensors. Inclou límits baixos i alts, prioritat, histèresi, retard d’activació i retard de retorn a normalitat.



\### `sensor\_readings`



Emmagatzema les lectures simulades dels sensors.



\### `alarm\_events`



Registra els esdeveniments d’alarma generats pel sistema.



\### `audit\_trail`



Registra accions rellevants sobre el sistema, com reconeixement o tancament d’alarmes.



\### `alarm\_pending\_conditions`



Permet gestionar condicions anormals pendents abans de generar una alarma activa, incorporant retard d’activació i lògica d’histèresi.



\## 4. Flux funcional general



El flux funcional simplificat és:



1\. El programa C++ connecta amb MySQL.

2\. Llegeix zones, sensors i regles d’alarma.

3\. Insereix lectures simulades.

4\. Avalua les lectures contra les regles configurades.

5\. Si una lectura surt de rang, crea o actualitza una condició pendent.

6\. Si la desviació persisteix més enllà del retard d’activació, genera una alarma `ACTIVE`.

7\. El sistema pot reconèixer l’alarma i registrar l’acció en `audit\_trail`.

8\. El sistema pot tancar l’alarma o marcar el retorn a normalitat.

9\. Finalment, genera informes en CSV i HTML.



\## 5. Gestió d’alarmes



El prototip incorpora diversos conceptes habituals en gestió d’alarmes industrials:



\* prioritat d’alarma;

\* alarmes `ACTIVE`;

\* reconeixement d’alarma;

\* tancament d’alarma;

\* control d’alarmes repetides;

\* histèresi;

\* retard d’activació;

\* retard de retorn a normalitat;

\* traçabilitat documental.



\## 6. Informes



El sistema genera informes orientats a revisió tècnica i documental:



\* informe CSV de traçabilitat;

\* informe CSV amb audit trail;

\* informe HTML final.



Aquests informes permeten revisar alarmes, sensors, zones, regles, valors d’activació, retorn a normalitat i accions registrades.



\## 7. Estat actual



L’arquitectura actual és funcional com a prototip de consola. Les dades de sensors són simulades i no provenen encara de dispositius físics.



\## 8. Possibles ampliacions



Possibles fases futures:



\* refactorització per classes separades;

\* integració amb sensors reals;

\* comunicació amb PLC o microcontroladors;

\* interfície gràfica;

\* gestió d’usuaris i rols;

\* signatura electrònica;

\* còpies de seguretat;

\* validació formal;

\* proves unitàries;

\* instal·lador o paquet portable.



