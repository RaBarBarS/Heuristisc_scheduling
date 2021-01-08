﻿// HeuristiscScheduling.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <ctime>
#include <stdio.h> 

clock_t start, stop;
//params
const double workingTime = 20;	//time in secs
const int iterations = 10;   //number of iterations to run
bool iterations = true; // set app to run given iterations number if true, if false app run given perioid of time
const int populationSize = 180; 
//given problem
int problem[2][problemSize];    //tasks times
int breakLen = 0;   //break length
int maxTimeBetweenBreaks = 0; //maximum time between two holes
//variables
float populationScores[populationSize]; //score of each solution (Cmax)
vector<vector<vector<int>>> solutions;  //solutions of current iteration
vector<int>parents; //parents of current solutions
vector<vector<int>> bestSolution;   //the best that has been found

int main()
{
    std::cout << "Hello World!\n";
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln