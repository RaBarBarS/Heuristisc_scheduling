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


//params
const double workingTime = 20;	//time in secs
const int iterations = 10;   //number of iterations to run
bool iterations = true; // set app to run given iterations number if true, if false app run given perioid of time
const int populationSize = 180; 
const int parentsForGeneration = 10;    //number of parents to be selected fo next generation
//given problem
int problem[2][problemSize];    //tasks times
int breakLen = 0;   //break length
int maxTimeBetweenBreaks = 0; //maximum time between two holes
//variables
clock_t start, stop;
float populationScores[populationSize]; //score of each solution (Cmax)
vector<vector<vector<int>>> solutions;  //solutions of current iteration
vector<int>parents; //parents of current solutions
vector<vector<int>> bestSolution;   //the best that has been found

int countCmax(int id) {
    int m2len = solutions[id][1].length();
    int cmax = 0;

    for (int i = 0; i < m2len; i++) {
        if (solutions[id][1][i] == 30000) {//30 000 break
            cmax += breakLen;
        }
        else if (solutions[id][1][i] >= 0) {//<0,29999> number of taks
            cmax += problem[1][solutions[id][1][i]];
        }
        else {//<-29999,-1>waiting time
            cmax += -1 * solutions[id][1][i];
        }
    }

    return cmax;
}

void roulette() {
    parents.clear();
    float sumOfScores = 0;
    float probabilitySum = 0;
    float probabilityOfParent[populationSize];

    for (int i = 0; i < populationSize; i++)    {
        sumOfScores += populationScores[i];
    }

    for (int i = 0; i < populationSize; i++) {
        probabilityOfParent[i] = (1 - (populationScores[i] / sum)) / sum;   //as we want the smallest score to have the biggest probability we have to substract from 1...
        probabilitySum += probabilityOfParent[i];   //probabilitest created like that won't sum to 1, so we have to sum them
    }

    for (int i = 0; i < parentsForGeneration; i++) {
        float x = fmod((double)(rand() / 100.0), probabilitySum);   //picking random number from (0,probabilitySum)
        int j = 0;
        sum = 0;
        while (sum < x) {
            sum += probabilityOfParent[j];
            j++;
        }
        if (j == populationSize)j--;    //just in case

        vector<int>::iterator it;
        it = find(parents.begin(), parents.end(), j);
        if (it != parents.end()) {	//check if this one has been already choosen
            i--;	//if so make more iterations to choose someone else
        }
        else {
            parents.push_back(j);
        }
    }
}

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
