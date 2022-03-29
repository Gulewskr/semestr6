- dynamic xpositive/2.
:- dynamic xnegative/2.
%1. Opis obiektów (ich cech charakterystycznych)
vehicle_is(car) :-
    it_is(road_vehicle) ,
    positive(is,medium) ,
    negative(has,wheels_2) ,
    positive(has,wheels_4) ,
    positive(has,engine).

vehicle_is(bus) :-
    it_is(road_vehicle) ,
    positive(is,big) ,
    negative(has,wheels_2) ,
    positive(has,wheels_4) ,
    positive(has,engine).

vehicle_is(train) :-
    it_is(rail_vehicle) ,
    positive(is,very_big) ,
    positive(has,engine).

vehicle_is(tram) :-
    it_is(rail_vehicle) ,
    positive(is,big) ,
    positive(has,engine).

vehicle_is(motorbike) :-
    it_is(road_vehicle) ,
    positive(has,wheels_2) ,
    negative(has,wheels_4) ,
    positive(has,engine).

vehicle_is(skateboard) :-
    it_is(road_vehicle) ,
    positive(is,small) ,
    negative(has,wheels_2) ,
    positive(has,wheels_4) ,
    negative(has,engine).

vehicle_is(bike) :-
    it_is(road_vehicle) ,
    positive(is,small) ,
    positive(has,wheels_2) ,
    negative(has,wheels_4) ,
    negative(has,engine).

vehicle_is(plane) :-
    it_is(air_vehicle) ,
    positive(is,very_big) ,
    positive(has,wings) ,
    positive(has,engine).

vehicle_is(balloon) :-
    it_is(air_vehicle) ,
    positive(is,big) ,
    negative(has,wings) ,
    positive(has,engine).

vehicle_is(ferry) :-
    it_is(water_vehicle) ,
    positive(is,very_big) ,
    positive(has,engine).

vehicle_is(canoe) :-
    it_is(water_vehicle) ,
    positive(is,small) ,
    negative(has,engine).

%2. Opis cech charakterystycznych dla klas obiektów
it_is(land_vehicle) :-
    positive(does,ride),
    negative(does,swim),
    negative(does,fly).

it_is(water_vehicle) :-
    negative(does,ride),
    positive(does,swim),
    negative(does,fly).

it_is(air_vehicle) :-
    negative(does,ride),
    negative(does,swim),
    positive(does,fly).

it_is(rail_vehicle) :-
    it_is(land_vehicle),
    negative(has, wheels).

it_is(road_vehicle) :-
    it_is(land_vehicle),
    positive(has, wheels).

%3. Szukanie potwierdzenia cechy obiektu w dynamicznej bazie
positive(X,Y) :-
    xpositive(X,Y),!.
positive(X,Y) :-
    not(xnegative(X,Y)) ,
    ask(X,Y,yes).
negative(X,Y) :-
    xnegative(X,Y),!.
negative(X,Y) :-
    not(xpositive(X,Y)) ,
    ask(X,Y,no).

%4. Zadawanie pytań użytkownikowi
ask(X,Y,yes) :-
    write(X), write(' it '),write(Y), write('\n'),
    read(Reply),
    sub_string(Reply,0,1,_,'y'),!,
    remember(X,Y,yes).

ask(X,Y,no) :-
    write(X), write(' it '),write(Y), write('\n'),
    read(Reply),
    sub_string(Reply,0,1,_, 'n'),!,
    remember(X,Y,no).

%5. Zapamiętanie odpowiedzi w dynamicznej bazie
remember(X,Y,yes) :-
    asserta(xpositive(X,Y)).

remember(X,Y,no) :-
    asserta(xnegative(X,Y)).

%6. Uruchomienie programu
run :-
    vehicle_is(X),!,
    write('\nYour vehicle may be a(n) '),write(X),
    nl,nl,clear_facts.

run :-
    write('\nUnable to determine what'),
    write('your vehicle is.\n\n'),clear_facts.

%7. Wyczyszczenie zawartości dynamicznej bazy
clear_facts :-
    retract(xpositive(_,_)),fail.
clear_facts :-
    retract(xnegative(_,_)),fail.
clear_facts :-
    write('\n\nPlease press the space bar to exit\n').