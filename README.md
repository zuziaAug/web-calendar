### Temat zadania
System współdzielonego kalendarza sieciowego, który umożliwia klientom zarządzanie eventami we wspólnym kalendarzu przechowywanym na serwerze.

### Komunikacja
Klienci komunikują się z serwerem przy pomocy protokołu TCP. Klient łączy się z serwerem, a następnie może wybrać jedną z dostępnych akcji.

### Implementacja
Projekt składa się z dwóch głównych części: serwera (server.cpp) i folderu klienta, w którym najważniejszym plikiem z funkcjonalnościami klienta jest plik mainwindow.cpp. GUI klienta zostało stworzone w framework'u Qt, przy użyciu Qt Creatora.

**server.cpp:**
Plik zawiera funkcje umożliwiające dodanie wydarzenia do kalendarza, usunięcie go, pokazanie wszystkich wydarzeń, pokazanie wszystkich aktywnych klientów, usunięcie klienta z listy aktywnych klientów, rejestrację klientów, zarządzającą działaniami klienta, parsowanie requesta oraz główną funkcję main, w której serwer łączy się z klientem i tworzy wątki.

**mainwindow.cpp:**
Plik zawiera funkcje do otrzymywania wątku oraz tworzenia requesta, a także cały szkielet GUI, który pozwala na wybieranie odpowiednich akcji i przesyłanie requestów do serwera.

### Kompilacja 

**Plik server.cpp:**
g++ -std=c++11 -Wall -o server server.cpp -lstdc++
**Uruchomienie:** ./server

**Dla niektórych urządzeń do poprawnej kompilacji wymagane jest dołączenie -pthread na końcu polecenia!**

W cel uruchomieniu klienta należy przejść do folderu **build**. W tym miejscu otworzyć terminal i uruchomić polecenie: **chmod u+x client_gui**. Bez tego, użytkownik nie będzie miał prawa wykonania danego pliku. Ze względu na stworzenie GUI w frameworku Qt (użycie Qt Creator), klient wymaga jedynie uruchomienia poprzez polecenie: **./client_gui**.

### Działanie programu
Po połączeniu z serwerem, klientowi wyświetla się okienko z opcjami do wyboru. Klient może zmienić port i nazwę serwera, a także nadać sobie ID. Po kliknięciu przycisku 'Connect', klient łączy się z serwerem. Wynik połączenia jest pokazywany zarówno na serwerze, jak i w okienku klienta. Niżej widnieje miejsce na wpisanie danych nowego wydarzenia do kalendarza i dodania go. Dodatkowo, klient może zobaczyć wszystkie istniejące w kalendarzu wydarzenia, a także wszystkich aktualnie połączonych klientów.
