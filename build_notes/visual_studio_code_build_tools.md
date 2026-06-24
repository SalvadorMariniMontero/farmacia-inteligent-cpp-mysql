\# Compilació amb Visual Studio Code i Build Tools 2022



Aquest document descriu l’entorn utilitzat per compilar el prototip \*\*Farmàcia Intel·ligent en C++ i MySQL\*\* en Windows.



El projecte s’ha desenvolupat com a prototip de consola en C++, connectat a MySQL mitjançant MySQL Connector/C++.



\## 1. Entorn utilitzat



Entorn de desenvolupament principal:



\* Windows 11

\* Visual Studio Code

\* Microsoft Visual Studio Build Tools 2022

\* Toolset MSVC v143

\* MySQL Server 8.0

\* MySQL Connector/C++

\* Git for Windows



Aquest repositori publica el codi font principal i la documentació del prototip. No publica binaris compilats ni carpetes temporals de compilació.



\## 2. Components necessaris



Per compilar el projecte cal tenir instal·lats:



1\. Microsoft Visual Studio Build Tools 2022.

2\. C++ build tools.

3\. Windows SDK.

4\. MySQL Server.

5\. MySQL Connector/C++.

6\. Visual Studio Code.



La instal·lació dels Build Tools pot fer-se des de Visual Studio Installer seleccionant la càrrega de treball de desenvolupament C++.



També es pot instal·lar mitjançant `winget`:



```cmd

winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --wait"

```



\## 3. Obrir l’entorn de compilació



Per compilar amb `cl` o `msbuild`, cal carregar abans l’entorn de Visual Studio.



Una opció és obrir el terminal de desenvolupament de Visual Studio.



Una altra opció és executar manualment:



```cmd

"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat"

```



Després es pot comprovar que el compilador està disponible amb:



```cmd

cl

```



o bé:



```cmd

msbuild -version

```



\## 4. Obrir Visual Studio Code des de l’entorn preparat



Després de carregar `VsDevCmd.bat`, es pot obrir Visual Studio Code des de la carpeta del projecte:



```cmd

cd C:\\Proyectos\\FarmaciaInteligenteCpp\_GitHub

code .

```



Això permet que el terminal integrat de VS Code hereti les variables d’entorn necessàries per compilar amb MSVC.



\## 5. Estructura del codi font



Els fitxers principals del prototip són:



```text

src/main.cpp

src/Database.h

src/Database.cpp

```



El programa principal crea una connexió amb MySQL, demana la contrasenya per consola i executa la cadena funcional del prototip.



\## 6. MySQL Connector/C++



El projecte necessita MySQL Connector/C++ per compilar.



Les rutes concretes poden variar segons la instal·lació. En una instal·lació típica poden existir carpetes semblants a:



```text

C:\\Program Files\\MySQL\\Connector C++ 8.0\\include

C:\\Program Files\\MySQL\\Connector C++ 8.0\\lib64\\vs14

```



o bé:



```text

C:\\Program Files\\MySQL\\MySQL Connector C++ 8.0\\include

C:\\Program Files\\MySQL\\MySQL Connector C++ 8.0\\lib64

```



Cal adaptar les rutes d’inclusió i llibreria segons la instal·lació real.



\## 7. Compilació conceptual amb MSVC



Si es compila manualment amb `cl`, cal indicar:



\* fitxers `.cpp`;

\* ruta d’inclusió del connector;

\* ruta de llibreries del connector;

\* llibreria del connector MySQL;

\* estàndard C++ utilitzat.



Exemple orientatiu:



```cmd

cl /EHsc /std:c++17 ^

&#x20; src\\main.cpp src\\Database.cpp ^

&#x20; /I"C:\\Program Files\\MySQL\\Connector C++ 8.0\\include" ^

&#x20; /link /LIBPATH:"C:\\Program Files\\MySQL\\Connector C++ 8.0\\lib64\\vs14" ^

&#x20; mysqlcppconn.lib

```



Aquest exemple pot requerir ajustos segons la versió exacta de MySQL Connector/C++ instal·lada.



\## 8. Compilació amb projecte Visual Studio



Durant el desenvolupament local, el prototip també es va compilar en configuració Release x64 mitjançant l’entorn de Microsoft Build Tools / Visual Studio.



En cas d’utilitzar un projecte `.vcxproj`, la compilació pot fer-se amb:



```cmd

msbuild NomDelProjecte.vcxproj /p:Configuration=Release /p:Platform=x64

```



Aquest repositori, en la seva versió pública inicial, prioritza el codi font, els scripts SQL i la documentació. Els fitxers de projecte Visual Studio poden afegir-se en una fase posterior si es vol facilitar una compilació directa.



\## 9. Execució del programa



Un cop compilat, el programa s’executa des de consola.



El sistema demana la contrasenya MySQL per entrada estàndard. Aquesta contrasenya no està escrita al codi font ni es publica al repositori.



Flux esperat:



```text

Enter MySQL password:

```



Després de connectar correctament, el programa pot executar les operacions implementades:



\* lectura de zones;

\* lectura de sensors;

\* lectura de regles;

\* generació de lectures simulades;

\* generació d’alarmes;

\* audit trail;

\* exportació CSV;

\* exportació HTML.



\## 10. Notes de seguretat



Aquest repositori no ha d’incloure:



\* contrasenyes;

\* fitxers `.env`;

\* binaris compilats;

\* carpetes `Debug`;

\* carpetes `Release`;

\* carpetes `x64`;

\* fitxers temporals de Visual Studio;

\* fitxers locals de configuració amb credencials.



El fitxer `.gitignore` està preparat per excloure aquests elements.



\## 11. Estat actual



Aquest document reflecteix l’entorn de compilació utilitzat durant el desenvolupament del prototip.



La compilació directa pot requerir ajustar les rutes de MySQL Connector/C++ i, eventualment, afegir fitxers de projecte o scripts de build en una fase futura.



