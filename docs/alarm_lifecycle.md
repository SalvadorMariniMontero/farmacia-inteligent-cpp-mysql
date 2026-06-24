\# Cicle de vida de les alarmes



Aquest document descriu el cicle funcional de les alarmes dins del prototip \*\*Farmàcia Intel·ligent en C++ i MySQL\*\*.



El sistema implementa una lògica simplificada però traçable per gestionar desviacions de sensors, alarmes actives, reconeixement, retorn a normalitat, tancament i registre documental.



\## 1. Objectiu



L’objectiu del cicle d’alarmes és transformar una lectura anormal de sensor en un esdeveniment documentat, revisable i traçable.



El prototip no es limita a detectar que un valor està fora de rang, sinó que incorpora criteris addicionals:



\* prioritat de l’alarma;

\* direcció de la desviació;

\* histèresi;

\* retard d’activació;

\* control d’alarmes repetides;

\* retorn a normalitat;

\* audit trail;

\* informes CSV i HTML.



\## 2. Elements principals



El cicle d’alarmes utilitza principalment aquestes taules:



\* `sensor\_readings`

\* `alarm\_rules`

\* `alarm\_pending\_conditions`

\* `alarm\_events`

\* `audit\_trail`



\## 3. Lectura de sensor



Cada lectura simulada queda registrada a `sensor\_readings`.



La lectura conté:



\* identificador de lectura;

\* sensor associat;

\* valor llegit;

\* data i hora;

\* font de la lectura.



Aquest registre és la base documental de qualsevol alarma posterior.



\## 4. Avaluació contra regles d’alarma



Cada lectura es compara amb les regles actives de `alarm\_rules`.



Una regla pot definir:



\* límit baix;

\* límit alt;

\* prioritat;

\* histèresi;

\* retard d’activació;

\* retard de retorn a normalitat.



Quan el valor llegit supera el límit alt, la desviació és de tipus `HIGH`.



Quan el valor llegit cau per sota del límit baix, la desviació és de tipus `LOW`.



\## 5. Condició pendent



El sistema no ha de generar necessàriament una alarma immediata davant la primera lectura fora de rang.



Per això existeix la taula `alarm\_pending\_conditions`.



Aquesta taula registra una desviació anormal encara pendent de confirmació. Això permet implementar retard d’activació i evitar alarmes massa sensibles davant desviacions momentànies.



Una condició pendent conté:



\* sensor;

\* regla;

\* direcció de l’alarma;

\* primer instant anormal;

\* últim instant anormal;

\* últim valor llegit;

\* font de la lectura.



\## 6. Activació de l’alarma



Si la desviació persisteix més enllà del retard d’activació configurat, el sistema crea una alarma a `alarm\_events`.



L’alarma queda inicialment en estat:



```text

ACTIVE

```



Aquest estat indica que existeix una situació anormal confirmada i pendent de gestió.



\## 7. Control d’alarmes repetides



El sistema evita generar múltiples alarmes obertes per la mateixa condició.



La detecció de duplicats es basa en:



\* regla;

\* sensor;

\* direcció de l’alarma;

\* estat de l’alarma.



Si ja existeix una alarma oberta equivalent, el sistema pot actualitzar-la en lloc de crear-ne una de nova.



Això redueix alarmes repetitives i millora la llegibilitat del sistema.



\## 8. Reconeixement de l’alarma



Una alarma `ACTIVE` pot passar a:



```text

ACKNOWLEDGED

```



Aquest estat indica que l’operador o usuari tècnic ha vist l’alarma i n’ha reconegut l’existència.



El reconeixement queda registrat en `audit\_trail`, indicant:



\* acció realitzada;

\* alarma afectada;

\* usuari;

\* data i hora;

\* canvi efectuat.



\## 9. Retorn a normalitat



Quan el sensor torna a un valor acceptable, el sistema pot marcar l’alarma com retornada a normalitat.



En aquest prototip s’utilitza l’estat:



```text

RETURNED

```



Aquest estat indica que la condició física o simulada que havia generat l’alarma ja no és present, encara que l’esdeveniment continua sent traçable.



El registre pot incloure:



\* valor de retorn;

\* moment de retorn;

\* últim valor anormal;

\* lectura origen.



\## 10. Tancament de l’alarma



Una alarma reconeguda pot ser tancada documentalment.



L’estat final pot ser:



```text

CLOSED

```



El tancament també es registra a `audit\_trail`, indicant qui ha tancat l’alarma i quan.



\## 11. Histèresi



La histèresi evita que una alarma canviï d’estat de manera inestable quan el valor del sensor oscil·la molt a prop del límit.



Per exemple, en una cambra freda amb límit alt de 8 °C, el sistema pot exigir que el valor baixi prou per sota del límit abans de considerar que la situació ha tornat realment a normalitat.



Aquesta lògica ajuda a reduir alarmes intermitents.



\## 12. Retard d’activació



El retard d’activació evita alarmes provocades per desviacions puntuals molt breus.



El sistema pot registrar primer una condició pendent i només generar una alarma activa si la desviació persisteix.



Això és especialment útil en sistemes de temperatura, humitat o vibració, on poden aparèixer transitoris curts.



\## 13. Retard de retorn a normalitat



El retard de retorn a normalitat permet confirmar que el valor ha tornat a una zona acceptable de manera estable, abans de considerar resolta la desviació.



Aquesta funció és útil per evitar canvis ràpids entre alarma activa i retorn a normalitat.



\## 14. Audit trail



El `audit\_trail` és la base documental del sistema.



Permet registrar accions com:



\* reconeixement d’alarma;

\* tancament d’alarma;

\* canvis d’estat;

\* usuari responsable;

\* data i hora de l’acció.



Això aporta traçabilitat i permet reconstruir què ha passat durant el cicle de vida d’una alarma.



\## 15. Informes



El sistema pot generar informes en CSV i HTML.



Aquests informes permeten revisar:



\* alarmes generades;

\* sensors implicats;

\* zones associades;

\* regles aplicades;

\* prioritats;

\* valors anormals;

\* retorn a normalitat;

\* accions d’audit trail.



\## 16. Resum del flux



Flux simplificat:



```text

Lectura simulada

&#x20;     ↓

Avaluació contra alarm\_rules

&#x20;     ↓

Condició normal o fora de rang

&#x20;     ↓

Condició pendent

&#x20;     ↓

Alarma ACTIVE

&#x20;     ↓

ACKNOWLEDGED

&#x20;     ↓

RETURNED o CLOSED

&#x20;     ↓

Informe CSV / HTML

```



\## 17. Estat actual del prototip



El cicle d’alarmes està implementat sobre lectures simulades i base de dades MySQL.



Encara no incorpora sensors físics, PLC, SCADA, interfície gràfica ni validació formal completa.



Tot i això, el flux ja permet demostrar els principis essencials d’una arquitectura d’alarmes traçable.



