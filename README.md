# Farmàcia Intel·ligent en C++ i MySQL

Prototip d’enginyeria desenvolupat en C++ i MySQL per simular un sistema d’alarmes, audit trail i traçabilitat documental aplicat a una farmàcia intel·ligent.

## Objectiu del projecte

L’objectiu és demostrar una arquitectura funcional capaç de:

* connectar una aplicació C++ amb una base de dades MySQL;
* llegir zones, sensors i regles d’alarma;
* inserir lectures simulades de sensors;
* generar alarmes quan una lectura surt dels límits configurats;
* gestionar el cicle d’una alarma;
* registrar accions en un audit trail;
* exportar informes de traçabilitat en CSV i HTML.

## Estat actual

El projecte es troba en fase de prototip funcional. S’han implementat les fases principals de lectura de dades, inserció de lectures simulades, generació d’alarmes, reconeixement, tancament, histèresi, retard d’activació, retorn a normalitat, audit trail i generació d’informes.

Aquesta versió no és un sistema GMP/GDP validat, sinó una demostració tècnica d’arquitectura software orientada a traçabilitat, alarmes i qualitat documental.

## Tecnologies utilitzades

* C++
* MySQL 8
* MySQL Connector/C++
* Visual Studio Code
* Microsoft Build Tools 2022
* Windows 11

## Funcionalitats implementades

* Connexió C++/MySQL.
* Lectura de zones d’emmagatzematge.
* Lectura de sensors.
* Lectura de regles d’alarma.
* Inserció de lectures simulades.
* Generació d’alarmes ACTIVE.
* Control d’alarmes repetides.
* Reconeixement d’alarmes.
* Tancament d’alarmes.
* Registre d’accions en audit trail.
* Histèresi i retard d’activació.
* Retorn a normalitat.
* Exportació CSV.
* Exportació HTML.

## Inspiració normativa i tècnica

El projecte s’ha inspirat en criteris generals de bones pràctiques relacionades amb:

* distribució farmacèutica GDP;
* sistemes informatitzats en entorns regulats;
* audit trail;
* gestió d’alarmes industrials;
* traçabilitat documental;
* control de temperatura en entornos farmacéuticos.

## Limitacions actuals

Aquesta versió encara no incorpora:

* sensors físics reals;
* PLC o microcontroladors;
* interfície gràfica;
* gestió avançada d’usuaris;
* signatura electrònica;
* validació formal completa;
* instal·lació automatitzada;
* proves unitàries estructurades.

## Treball futur

Possibles línies de continuació:

* refactorització en classes separades;
* integració amb sensors reals;
* comunicació amb PLC o dispositius IoT;
* interfície gràfica;
* sistema d’usuaris i rols;
* matriu de requisits i proves;
* documentació de validació;
* millora dels informes;
* publicació d’una versió demostrativa més portable.

## Autor

Salvador Marín i Montero
Enginyer mecànic retirat
Website: salvadormarinimontero.com
