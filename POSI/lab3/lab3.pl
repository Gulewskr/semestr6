:-dynamic komputer/5. 

wykPrg:-
 write('1 - biezacy stan bazy danych'), nl,
 write('2 - dopisanie nowego komputera'), nl,
 write('3 - usuniecie komputera'), nl,
 write('4 - obliczenie sredniej ceny'), nl,
 write('5 - uzupelnienie bazy o dane zapisane w pliku'), nl,
 write('6 - zapisanie bazy w pliku'), nl,
 write('7 - znajdz po nazwie procesora'), nl,
 write('8 - znajdz po cenie'), nl,
 write('0 - koniec programu'), nl, nl,
 read(I),
 I > 0,
 opcja(I),
 wykPrg.
wykPrg.
opcja(1) :- wyswietl.
opcja(2) :- write('Podaj nazwe procesora:'), read(Nazwa), nl,
 write('Podaj typ procesora:'), read(Typ), nl,
 write('Podaj czestotliwosc zegara:'), read(Czestotliwosc), nl,
 write('Podaj rozmiar HDD:'), read(Rozmiar), nl,
 write('Podaj Cene:'), read(Cena), nl,
 assert(komputer(Nazwa, Typ, Czestotliwosc, Rozmiar, Cena)).
opcja(3) :- write('Podaj nazwe usuwanego procesora:'), read(Nazwa),
 retract(komputer(Nazwa,_,_,_,_)),! ;
 write('Brak takiego komputera').
opcja(4) :- sredniaCena.
opcja(5) :- write('Podaj nazwe pliku:'), read(Nazwa),
 exists_file(Nazwa), !, consult(Nazwa);
 write('Brak pliku o podanej nazwie'), nl.
opcja(6) :- write('Podaj nazwe pliku:'), read(Nazwa),
 open(Nazwa, write, X), zapis(X), close(X).
opcja(7) :- znajdzPoNazwie.
opcja(8) :- podajCene.
opcja(_) :- write('Zly numer opcji'), nl.
wyswietl :- write('elementy bazy:'), nl,
 komputer(Nazwa, Typ, Czestotliwosc, Rozmiar, Cena),
 write(Nazwa), write(' '), write(Typ), write(' '), write(Czestotliwosc),
 write(' '), write(Rozmiar), write(' '), write(Cena), nl, fail.
wyswietl :- nl.
sredniaCena :- 
 findall(Cena, komputer(_,_,_,_,Cena), Lista),
 suma(Lista, Suma, LiczbaKomputerow),
 AvgPrice is Suma / LiczbaKomputerow,
 write('Srednia Cena:'), 
 write(AvgPrice), nl, nl.
 
 
zapis(X) :- komputer(Nazwa, Typ, Czestotliwosc, Rozmiar, Cena),
 write(X, 'komputer('),
 write(X, Nazwa), write(X, ','), write(X, Typ),
 write(X, Czestotliwosc), write(X, ','), write(X, Rozmiar),
 write(X, Cena), write(X, ','),
 write(X, ').'), nl(X), fail.
zapis(_) :- nl.
suma([],0,0).
suma([G|Og], Suma, N) :- suma(Og, S1, N1),
 Suma is G + S1,
 N is N1+1.

znajdzPoNazwie:- 
  write('Podaj nazwe procesora'),
  read(NazwaProcesora),
  setof(NazwaProcesora/Typ/Czestotliwosc/Rozmiar/Cena, komputer(NazwaProcesora,Typ,Czestotliwosc,Rozmiar,Cena),Lista),
  write('Znalezionych elementow: '),
  list_length(Lista, L),
  write(L),
  nl,
  write('Procesor/Typ/Czestotliwosc/Rozmiar/Cena'),
  nl,
  wypiszElementy(Lista).
  
podajCene:-
  write('Podaj budzet cenowy'),
  read(PodanaCena),
  znajdzPoCenie(PodanaCena).
  
znajdzPoCenie(PodanaCena):- 
  findall(Nazwa/Typ/Czestotliwosc/Rozmiar/Cena, (komputer(Nazwa,Typ,Czestotliwosc,Rozmiar,Cena), Cena < PodanaCena),Lista),
  write('Znalezionych elementow: '),
  list_length(Lista, L),
  write(L),
  nl,
  write('Element/Typ/Czestotliwosc/Rozmiar/Cena'),
  nl,
  wypiszElementy(Lista),
  
wypiszElementy([]).
wypiszElementy([A|B]):- 
	write(A),
	nl,
	wypiszElementy(B),
	nl.

	
list_length([]     , 0 ).
list_length([_|Xs] , L ) :- list_length(Xs,N) , L is N+1 .

list_min([L|Ls], Min) :-
    list_min(Ls, L, Min).

list_min([], Min, Min).
list_min([L|Ls], Min0, Min) :-
    Min1 is min(L, Min0),
    list_min(Ls, Min1, Min).
	
komputer(i54590, r4, 3300, 500, 2000).
komputer(i54590, r4, 3300, 500, 2100).
komputer(i54590, r4, 3300, 500, 2200).
komputer(i24590, r2, 300, 200, 1000).