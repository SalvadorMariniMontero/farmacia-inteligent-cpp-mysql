\# Configuració de MySQL Connector/C++



Aquest document descriu la configuració general de \*\*MySQL Connector/C++\*\* utilitzada pel prototip \*\*Farmàcia Intel·ligent en C++ i MySQL\*\*.



El connector és necessari perquè l’aplicació C++ pugui establir connexió amb MySQL, executar consultes SQL i recuperar resultats.



\## 1. Objectiu



L’objectiu d’aquesta configuració és permetre que el programa C++ pugui:



\* connectar amb MySQL Server;

\* seleccionar l’esquema `farmacia\_inteligente`;

\* consultar zones, sensors i regles d’alarma;

\* inserir lectures simulades;

\* crear i actualitzar alarmes;

\* generar audit trail;

\* exportar informes.



\## 2. Components necessaris



Cal tenir instal·lats:



\* MySQL Server 8.0 o compatible;

\* MySQL Connector/C++;

\* compilador MSVC;

\* Visual Studio Build Tools 2022 o Visual Studio Community;

\* Windows SDK.



\## 3. Rutes habituals d’instal·lació



Les rutes poden variar segons la versió instal·lada.



Exemples habituals:



```text

C:\\Program Files\\MySQL\\Connector C++ 8.0\\

```



o bé:



```text

C:\\Program Files\\MySQL\\MySQL Connector C++ 8.0\\

```



Dins d’aquestes carpetes solen existir subcarpetes com:



```text

include

lib64

lib64\\vs14

```



\## 4. Capçaleres utilitzades



El codi pot requerir capçaleres del connector semblants a:



```cpp

\#include <mysql\_driver.h>

\#include <mysql\_connection.h>

\#include <cppconn/driver.h>

\#include <cppconn/connection.h>

\#include <cppconn/statement.h>

\#include <cppconn/prepared\_statement.h>

\#include <cppconn/resultset.h>

\#include <cppconn/exception.h>

```



La ruta `include` del connector ha d’estar disponible per al compilador.



\## 5. Llibreries necessàries



Durant l’enllaç, el compilador ha de trobar la llibreria del connector.



En instal·lacions clàssiques pot ser:



```text

mysqlcppconn.lib

```



També pot ser necessari tenir disponible en temps d’execució el fitxer DLL corresponent, per exemple:



```text

mysqlcppconn.dll

```



La ruta exacta depèn de la instal·lació del connector.



\## 6. Exemple conceptual de compilació



Exemple orientatiu amb MSVC:



```cmd

cl /EHsc /std:c++17 ^

&#x20; src\\main.cpp src\\Database.cpp ^

&#x20; /I"C:\\Program Files\\MySQL\\Connector C++ 8.0\\include" ^

&#x20; /link /LIBPATH:"C:\\Program Files\\MySQL\\Connector C++ 8.0\\lib64\\vs14" ^

&#x20; mysqlcppconn.lib

```



Aquest exemple pot requerir adaptacions segons:



\* versió concreta del connector;

\* ruta real d’instal·lació;

\* arquitectura x64;

\* configuració Debug o Release;

\* ús de Visual Studio Community o Build Tools.



\## 7. Arquitectura x64



El prototip s’ha treballat en configuració x64.



És important mantenir coherència entre:



\* compilador x64;

\* llibreries x64;

\* MySQL Connector/C++ x64;

\* MySQL Server instal·lat.



Barrejar components x86 i x64 pot provocar errors d’enllaç o execució.



\## 8. Connexió amb MySQL



El programa demana la contrasenya per consola i crea una connexió amb MySQL.



Es recomana no escriure contrasenyes dins del codi font.



Exemple conceptual:



```cpp

connection\_.reset(driver\_->connect(host\_, user\_, password\_));

connection\_->setSchema(schema\_);

```



En el prototip, l’usuari de base de dades utilitzat és:



```text

farmacia\_app

```



La contrasenya no es publica al repositori.



\## 9. Host de connexió



Durant el desenvolupament es pot utilitzar:



```text

localhost

```



o bé:



```text

127.0.0.1

```



Cal tenir en compte que MySQL pot distingir entre usuaris definits com:



```text

'farmacia\_app'@'localhost'

```



i:



```text

'farmacia\_app'@'127.0.0.1'

```



Per aquest motiu, alguns permisos poden funcionar en una forma de connexió i no en una altra.



\## 10. Permisos recomanats per a l’usuari de l’aplicació



L’usuari de l’aplicació hauria de tenir només els permisos necessaris.



Per a un prototip local, pot necessitar permisos sobre l’esquema:



```sql

SELECT, INSERT, UPDATE

```



En algunes fases de desenvolupament també poden ser necessaris permisos addicionals per crear o modificar taules, però en un sistema més madur aquests permisos haurien d’estar restringits.



\## 11. Prova bàsica de connexió



Una prova mínima de connexió consisteix en:



1\. connectar amb MySQL;

2\. seleccionar l’esquema `farmacia\_inteligente`;

3\. executar una consulta senzilla;

4\. mostrar la versió del servidor o una llista de zones.



Exemple de consulta:



```sql

SELECT VERSION();

```



o bé:



```sql

SELECT zone\_code, name FROM storage\_zones ORDER BY zone\_code;

```



\## 12. Errors freqüents



\### Error: no es troben capçaleres



Possible causa:



\* ruta `include` incorrecta;

\* connector no instal·lat;

\* projecte no configurat amb la ruta del connector.



\### Error: no es troba `mysqlcppconn.lib`



Possible causa:



\* ruta `lib64` incorrecta;

\* arquitectura x86/x64 inconsistent;

\* llibreria no instal·lada.



\### Error en temps d’execució per DLL absent



Possible causa:



\* `mysqlcppconn.dll` no és al directori de l’executable;

\* la ruta del connector no és al `PATH`.



Solucions possibles:



\* copiar la DLL al costat de l’executable;

\* afegir la carpeta de la DLL al `PATH`;

\* configurar correctament l’entorn de Visual Studio.



\### Error d’accés a MySQL



Possible causa:



\* usuari sense permisos;

\* contrasenya incorrecta;

\* diferència entre `localhost` i `127.0.0.1`;

\* esquema incorrecte;

\* servei MySQL aturat.



\## 13. Bones pràctiques



Recomanacions aplicades o previstes:



\* no publicar contrasenyes;

\* no publicar fitxers locals de configuració;

\* usar `.gitignore`;

\* separar codi font, scripts SQL i documentació;

\* documentar l’entorn de compilació;

\* limitar permisos de l’usuari MySQL;

\* mantenir traçabilitat de canvis amb Git.



\## 14. Estat actual



Aquest document resumeix la configuració necessària per entendre com el prototip C++ es comunica amb MySQL.



En futures fases es podria afegir:



\* fitxer de projecte Visual Studio;

\* script de compilació;

\* CMake;

\* instruccions completes d’instal·lació;

\* configuració amb variables d’entorn;

\* usuari MySQL de només demostració.



