# \# Farmàcia Intel·ligent en C++ i MySQL

# 

# Prototip d’enginyeria desenvolupat en \*\*C++\*\* i \*\*MySQL\*\* per simular un sistema d’alarmes, audit trail i traçabilitat documental aplicat a una farmàcia intel·ligent.

# 

# Aquest projecte forma part d’una línia de treball personal orientada a integrar software, bases de dades, criteris de qualitat documental i automatització en un context farmacèutic simulat.

# 

# Article publicat al website:

# 

# \[Farmàcia intel·ligent en C++ i MySQL – Prototip d’alarmes, audit trail i traçabilitat documental](https://www.salvadormarinimontero.com/projectes-2/farmacia-intelligent-en-c-i-mysql-prototip-dalarmes-audit-trail-i-tracabilitat-documental/)

# 

# \---

# 

# \## 1. Objectiu del projecte

# 

# L’objectiu és demostrar una arquitectura funcional capaç de:

# 

# \* connectar una aplicació C++ amb una base de dades MySQL;

# \* llegir zones d’emmagatzematge, sensors i regles d’alarma;

# \* inserir lectures simulades de sensors;

# \* generar alarmes quan una lectura surt dels límits configurats;

# \* aplicar histèresi i retard d’activació;

# \* controlar alarmes repetides;

# \* gestionar el cicle documental d’una alarma;

# \* registrar accions en un audit trail;

# \* exportar informes de traçabilitat en CSV i HTML.

# 

# El projecte no pretén ser un sistema farmacèutic validat, sinó una \*\*demostració tècnica d’arquitectura software\*\* orientada a traçabilitat, alarmes i qualitat documental.

# 

# \---

# 

# \## 2. Estat actual

# 

# El projecte es troba en fase de \*\*prototip funcional publicable\*\*.

# 

# S’han implementat les fases principals de:

# 

# \* connexió C++ / MySQL;

# \* lectura de configuració;

# \* inserció de lectures simulades;

# \* generació d’alarmes;

# \* reconeixement i tancament d’alarmes;

# \* audit trail;

# \* control de duplicats;

# \* histèresi;

# \* retard d’activació;

# \* retorn a normalitat;

# \* exportació CSV;

# \* exportació HTML;

# \* documentació pública inicial del repositori.

# 

# Aquesta versió no és un sistema GMP/GDP validat. És una base experimental d’enginyeria per explorar com podria estructurar-se un sistema traçable de supervisió i alarmes.

# 

# \---

# 

# \## 3. Tecnologies utilitzades

# 

# \* C++

# \* MySQL 8

# \* MySQL Connector/C++

# \* Visual Studio Code

# \* Microsoft Visual Studio Build Tools 2022

# \* MSVC v143

# \* Git / GitHub

# \* Windows 11

# 

# \---

# 

# \## 4. Estructura del repositori

# 

# ```text

# farmacia-inteligent-cpp-mysql/

# │

# ├── README.md

# ├── .gitignore

# │

# ├── src/

# │   ├── main.cpp

# │   ├── Database.h

# │   └── Database.cpp

# │

# ├── sql/

# │   ├── 01\_create\_schema.sql

# │   ├── 02\_insert\_demo\_data.sql

# │   └── 03\_example\_queries.sql

# │

# ├── docs/

# │   ├── architecture.md

# │   ├── alarm\_lifecycle.md

# │   └── screenshots/

# │

# ├── build\_notes/

# │   ├── visual\_studio\_code\_build\_tools.md

# │   └── mysql\_connector\_cpp\_setup.md

# │

# └── reports/

# ```

# 

# \---

# 

# \## 5. Codi font

# 

# El codi principal es troba a:

# 

# ```text

# src/main.cpp

# src/Database.h

# src/Database.cpp

# ```

# 

# La classe `Database` concentra la connexió amb MySQL i les funcions principals del prototip.

# 

# El programa demana la contrasenya MySQL per consola. La contrasenya no està escrita al codi font ni es publica al repositori.

# 

# \---

# 

# \## 6. Base de dades

# 

# Els scripts SQL es troben a la carpeta:

# 

# ```text

# sql/

# ```

# 

# Fitxers inclosos:

# 

# ```text

# 01\_create\_schema.sql

# 02\_insert\_demo\_data.sql

# 03\_example\_queries.sql

# ```

# 

# Taules principals:

# 

# \* `storage\_zones`

# \* `sensors`

# \* `alarm\_rules`

# \* `sensor\_readings`

# \* `alarm\_events`

# \* `audit\_trail`

# \* `alarm\_pending\_conditions`

# 

# \### Zones demostratives

# 

# El prototip utilitza zones com:

# 

# \* zona ambient;

# \* cambra freda;

# \* zona robotitzada.

# 

# \### Sensors demostratius

# 

# Exemples de sensors:

# 

# \* temperatura ambient;

# \* humitat ambient;

# \* temperatura de cambra freda;

# \* vibració de zona robotitzada.

# 

# \### Regles d’alarma

# 

# Les regles defineixen:

# 

# \* sensor associat;

# \* prioritat;

# \* límit baix;

# \* límit alt;

# \* histèresi;

# \* retard d’activació;

# \* retard de retorn a normalitat;

# \* estat actiu/inactiu.

# 

# \---

# 

# \## 7. Cicle funcional d’alarmes

# 

# El sistema segueix un flux simplificat:

# 

# ```text

# Lectura simulada

# &#x20;     ↓

# Avaluació contra alarm\_rules

# &#x20;     ↓

# Condició normal o fora de rang

# &#x20;     ↓

# Condició pendent

# &#x20;     ↓

# Alarma ACTIVE

# &#x20;     ↓

# ACKNOWLEDGED

# &#x20;     ↓

# RETURNED o CLOSED

# &#x20;     ↓

# Informe CSV / HTML

# ```

# 

# El document específic és:

# 

# ```text

# docs/alarm\_lifecycle.md

# ```

# 

# Aquest document explica:

# 

# \* condicions pendents;

# \* activació d’alarmes;

# \* control de duplicats;

# \* reconeixement;

# \* retorn a normalitat;

# \* tancament;

# \* audit trail;

# \* informes.

# 

# \---

# 

# \## 8. Arquitectura

# 

# La documentació d’arquitectura es troba a:

# 

# ```text

# docs/architecture.md

# ```

# 

# El sistema es pot entendre com tres capes:

# 

# 1\. aplicació C++ de consola;

# 2\. base de dades MySQL;

# 3\. informes documentals CSV / HTML.

# 

# Aquesta arquitectura permet demostrar una cadena mínima de traçabilitat:

# 

# ```text

# Sensor simulat → Lectura → Regla → Alarma → Audit trail → Informe

# ```

# 

# \---

# 

# \## 9. Informes generats

# 

# El prototip pot generar informes de traçabilitat en:

# 

# \* CSV;

# \* HTML.

# 

# Aquests informes permeten revisar:

# 

# \* alarmes generades;

# \* sensors implicats;

# \* zones associades;

# \* regles aplicades;

# \* prioritats;

# \* valors d’activació;

# \* retorn a normalitat;

# \* accions registrades en audit trail.

# 

# La carpeta `reports/` queda reservada per a exemples públics d’informes.

# 

# \---

# 

# \## 10. Entorn de compilació

# 

# Les notes de compilació es troben a:

# 

# ```text

# build\_notes/

# ```

# 

# Documents disponibles:

# 

# ```text

# visual\_studio\_code\_build\_tools.md

# mysql\_connector\_cpp\_setup.md

# ```

# 

# Aquests documents expliquen:

# 

# \* ús de Visual Studio Code;

# \* ús de Build Tools 2022;

# \* configuració de MySQL Connector/C++;

# \* consideracions sobre rutes d’inclusió i llibreries;

# \* possibles errors freqüents.

# 

# \---

# 

# \## 11. Inspiració normativa i tècnica

# 

# El projecte s’ha inspirat en criteris generals de bones pràctiques relacionades amb:

# 

# \* distribució farmacèutica GDP;

# \* sistemes informatitzats en entorns regulats;

# \* audit trail;

# \* gestió d’alarmes industrials;

# \* traçabilitat documental;

# \* control de temperatura en entorns farmacèutics;

# \* criteris de qualitat de dades;

# \* gestió del risc proporcional.

# 

# Aquesta inspiració és conceptual i tècnica. El projecte no afirma compliment regulatori formal.

# 

# \---

# 

# \## 12. Limitacions actuals

# 

# Aquesta versió encara no incorpora:

# 

# \* sensors físics reals;

# \* PLC;

# \* microcontroladors;

# \* SCADA;

# \* interfície gràfica;

# \* gestió avançada d’usuaris;

# \* signatura electrònica;

# \* matriu formal de requisits i proves;

# \* validació GMP/GDP completa;

# \* instal·lació automatitzada;

# \* proves unitàries estructurades;

# \* paquet portable de compilació.

# 

# \---

# 

# \## 13. Treball futur

# 

# Possibles línies de continuació:

# 

# \* refactorització en classes separades;

# \* separació de models, serveis i repositoris;

# \* integració amb sensors reals;

# \* comunicació amb PLC o dispositius IoT;

# \* interfície gràfica;

# \* gestió d’usuaris i rols;

# \* exportació millorada d’informes;

# \* incorporació de proves unitàries;

# \* ús de CMake;

# \* fitxers de configuració sense credencials;

# \* matriu de requisits i proves;

# \* documentació de validació;

# \* còpies de seguretat;

# \* desplegament local reproduïble.

# 

# \---

# 

# \## 14. Advertiment important

# 

# Aquest repositori és un \*\*prototip educatiu i d’enginyeria conceptual\*\*.

# 

# No s’ha d’utilitzar directament en un entorn farmacèutic real sense:

# 

# \* validació formal;

# \* qualificació d’infraestructura;

# \* control d’accessos;

# \* gestió documental;

# \* proves de seguretat;

# \* revisió regulatòria;

# \* verificació de dades;

# \* gestió completa de canvis;

# \* procediments operatius aprovats.

# 

# \---

# 

# \## 15. Autor

# 

# \*\*Salvador Marín i Montero\*\*

# Enginyer mecànic retirat

# 

# Website personal:

# 

# https://www.salvadormarinimontero.com

# 

# Repositori GitHub:

# 

# https://github.com/SalvadorMariniMontero/farmacia-inteligent-cpp-mysql

# 

# \---

# 

# \## ## 16. Llicència

Aquest projecte es publica sota llicència MIT.

La llicència permet consultar, utilitzar, modificar i reutilitzar el codi, sempre mantenint l’avís de copyright i la llicència original.

Vegeu el fitxer:

```text
LICENSE
```

