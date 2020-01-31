# Kompilator - JFTT 2019/2020

Repozytorium to zawiera projekt kompilatora prostego języka imperatywnego do kodu maszyny wirtualnej, stworzony w ramach kursu <b>Języki Formalne i Techniki Translacji</b>. Jest to przedmiot realizowany dla studentów trzeciego roku informatyki na wydziale WPPT Politechniki Wrocławskiej. Wykładowcą jest <b>dr Maciej Gębala</b>. 

## Kompilacja
W celu kompilacji plików źródłowych należy przejść do katalogu `kompilator/src` i użyć polecenia `make`.

Aby usunąć pliki powstałe w wyniku kompilacji należy wywołać polecenie `make clean`.

## Uruchamianie kompilatora
W celu uruchomienia kompilatora należy użyć poniższego polecenia:

``./kompilator plik_wejsciowy.imp plik_wyjsciowy.mr``

Jeżeli kompilacja przebiegnie pomyślnie zostanie zwrócony plik z kodem na maszynę wirtualną. W razie wystąpienia błędów w kodzie wejściowym, zostaną wypisane komunikaty o rodzaju znalezionych błędów oraz miejscu ich występowania. W takim przypadku kompilator nie zwraca pliku z kodem wynikowym.

## Interpreter
Do dyspozycji studentów został oddany interpreter prostego kodu maszynowego autorstwa <b>dra Macieja Gębali</b>, który znajduje się  w folderze `maszyna_wirtualna` wraz z plikiem README opisującym sposób kompilacji i uruchomienia.

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


