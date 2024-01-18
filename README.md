### Temat zadania
System współdzielonego kalendarza sieciowego, który umożliwia klientom zarządzanie eventami we wspólnym kalendarzu przechowywanym na serwerze.

### Komunikacja
Klienci komunikują się z serwerem przy pomocy protokołu TCP. Klient łączy się z serwerem, a następnie może wybrać jedną z dostępnych akcji.

### Implementacja
Projekt składa się z dwóch głównych części: serwera (server.cpp) i klienta (client.cpp).

**server.cpp:**
Plik zawiera funkcje umożliwiające dodanie wydarzenia do kalendarza, usunięcie go, pokazanie wszystkich wydarzeń, pokazanie wszystkich aktywnych klientów, usunięcie klienta z listy aktywnych klientów, rejestrację klientów, zarządzającą działaniami klienta, parsowanie requesta oraz główną funkcję main, w której serwer łączy się z klientem i tworzy wątki.

**client.cpp:**
Plik zawiera funkcje do otrzymywania wątku oraz tworzenia requesta, a główne działanie widnieje w funkcji main, w której powstaje menu dostępne dla klienta i przekazuje odpowiednie dane do serwera.

### Kompilacja 

**Plik client.cpp:**
g++ -std=c++11 -Wall -o client client.cpp -lstdc++
**Uruchomienie:** ./client id (np. ./client 0)

**Plik server.cpp:**
g++ -std=c++11 -Wall -o server server.cpp -lstdc++
**Uruchomienie:** ./server

**Dla niektórych urządzeń do poprawnej kompilacji wymagane jest dołączenie -pthread na końcu polecenia!**

### Działanie programu
Po połączeniu z serwerem, klientowi wyświetla się menu z opcjami do wyboru:
1. Add event
2. Delete event
3. Show all events
4. Show all clients
5. Exit

Po wybraniu konkretnej pozycji z menu, użytkownikowi wyświetlane są dalsze kroki. 
