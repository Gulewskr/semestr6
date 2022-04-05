#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 100000
#define S (int)sqrt(N)
#define M N / 10

int main(int argc, char **argv)
{

	int numberOfThreads = argc > 1 ? atoi(argv[1]) : 2;
	printf("liczba watkow = = %d\n", numberOfThreads);
	long int a[S + 1];	  /*tablica pomocnicza*/
	long int pierwsze[M]; /*liczby pierwsze w przedziale 2..N*/
	long i, k, liczba, reszta;
	long int lpodz;		 /* l. podzielnikow*/
	long int llpier = 0; /*l. liczb pierwszych w tablicy pierwsze*/
	double czas;		 /*zmienna do  mierzenia czasu*/
	FILE *fp;

/* 
	Notes:
		- nie testowałem jeszcze, więc nie wiem czy działa XD
		- te schedule można testować różne jak na wykładzie było
		- zapis do pliku usunąłem bo nic nie było w zadaniu
		- mierzenie czasu będzie trzeba dodać to pewnie jutro albo wieczorem jak przetestuje czy działa
*/
	omp_set_num_threads(numberOfThreads);
#pragma omp parallel 
{
	// część 1 wyznaczenie liczb pierwszych z przedziału 2 ... (int)sqrt(N)
	// inicjowanie
	#pragma omp for shared(a) private(i) schedule(dynamic)
	for (i = 2; i <= S; i++)
		a[i] = 1;

	// wykreślenie
	#pragma omp for shared(a, pierwsze, llpier) private(i, k) schedule(dynamic)
	for (i = 2; i <= S; i++)
		if (a[i] == 1)
		{
			#pragma omp critical
			{
				pierwsze[llpier++] = i;
			}
			for (k = i + i; k <= S; k += i)
				a[k] = 0;
		}

	lpodz = llpier; /*zapamietanie liczby podzielnikow*/
	
	//część 2 wyznaczenie liczb pierwszych z przedziału 2 ... (int)sqrt(N)
	#pragma omp for shared(pierwsze, llpier) private(liczba, reszta, k) schedule(dynamic)
	for (liczba = S + 1; liczba <= N; liczba++)
	{
		for (k = 0; k < lpodz; k++)
		{
			reszta = (liczba % pierwsze[k]);
			if (reszta == 0)
				break; /*liczba zlozona*/
		}
		if (reszta != 0)
		{
			#pragma omp critical
			{
				//zapamiętanie critical żeby tylko 1 liczba była zapisywana jednocześnie
				pierwsze[llpier++] = liczba; 
			}
		}
	}

}

	/* 
	// zapis do pliku - nie wiem czy to potrzebne
	if ((fp = fopen("primes.txt", "w")) == NULL)
	{
		printf("Nie moge otworzyc pliku do zapisu\n");
		exit(1);
	}
	for (i = 0; i < llpier; i++)
		fprintf(fp, "%ld ", pierwsze[i]);
	fclose(fp);
	*/
	return 0;
}
