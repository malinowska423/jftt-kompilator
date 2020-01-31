# Kompilator - JFTT 2019/2020

Repozytorium to zawiera projekt kompilatora prostego języka imperatywnego do kodu maszyny wirtualnej, stworzony w ramach kursu <b>Języki Formalne i Techniki Translacji</b>. Jest to przedmiot realizowany dla studentów trzeciego roku informatyki na wydziale WPPT Politechniki Wrocławskiej. Wykładowcą jest <b>dr Maciej Gębala</b>. 

### W skrócie
- autorka projektu: <b>Aleksandra Malinowska</b>
- język kompilatora: <b>C++</b>
- dodatkowe narzędzia: <b>Flex</b> oraz <b>Bison</b>

## Zawartość
Pliki źródłowe kompilatora znajdują się w katalogu `kompilator/src`. Poniżej przedstawiam opis poszczególnych plików.
- `compiler.hpp` - plik nagłówkowy zawierający deklaracje struktur oraz funkcji używanych przy tworzeniu kodu wynikowego na maszynę wirtualną
- `compiler.cpp` - plik zawierający rozwinięcie funkcji zadeklarowanych w pliku nagłówkowym 
- `lexer.l` - analizator leksykalny
- `Makefile` - program budujący plik wynikowy kompilatora
- `parser.y` - analizator składniowy
- `symbol-table.hpp` - plik nagłówkowy zawierający deklaracje struktury tablicy symboli kompilatora oraz funkcji potrzebnych do deklaracji zmiennych
- `symbol-table.cpp` - plik zawierający rozwinięcie funkcji zadeklarowanych w pliku nagłówkowym

W katalogu `kompilator/maszyna_wirtualna` znajduje się interpreter kodu wynikowego z kompilatora autorstwa <b>dra Macieja Gębali</b>.


## Kompilacja
W celu kompilacji plików źródłowych należy przejść do katalogu `kompilator/src` i użyć polecenia `make`.

Aby usunąć pliki powstałe w wyniku kompilacji należy wywołać polecenie `make clean`.

## Uruchamianie kompilatora
W celu uruchomienia kompilatora należy użyć poniższego polecenia:

``./kompilator plik_wejsciowy.imp plik_wyjsciowy.mr``

Jeżeli kompilacja przebiegnie pomyślnie zostanie zwrócony plik z kodem na maszynę wirtualną. W razie wystąpienia błędów w kodzie wejściowym, zostaną wypisane komunikaty o rodzaju znalezionych błędów oraz miejscu ich występowania. W takim przypadku kompilator nie zwraca pliku z kodem wynikowym.

## Przykład programu

```
[ sito Eratostenesa ]
DECLARE
    n, j, sito(2:100)
BEGIN
    n ASSIGN 100;
    FOR i FROM n DOWNTO 2 DO
        sito(i) ASSIGN 1;
    ENDFOR
    FOR i FROM 2 TO n DO
        IF sito(i) NEQ 0 THEN
            j ASSIGN i PLUS i;
            WHILE j LEQ n DO
                sito(j) ASSIGN 0;
                j ASSIGN j PLUS i;
            ENDWHILE
            WRITE i;
        ENDIF
    ENDFOR
END
```


