// HeuristiscScheduling.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
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
int problemSize = 0;
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

void nextGen() {//taking task order form parents    ///spr numerki od zadań, przerw i wymuszonych przerw
    int parent1 = 0, parent2 = 0;
    int percentageOfParent = 0;

    for (int i = 0; i < populationSize; i++) {
        parent1 = rand() % parents.size();
        parent2 = rand() % parents.size();
        percentageOfParent = (rand() % 20) + 40;    //rand form (40,59)

        while (parent1 == parent2) {
            parent2 = rand() % parents.size();
        }

        for (int k = 0; k < 2; k++) {
            vector<int>newSolution; //vector for new solution
            bool alreadyInNew[problemSize];  //write here what you take from praent1, so you know what to take form parent2
            for (int i = 0; i < problemSize; i++) {
                alreadyInNew[i] = false;
            }

            bool isThatAll = false;
            int numberTasksFromParent = percentageOfParent * problemSize / 100;

            for (int j = 0, m = 0; j < numberTasksFromParent + 1, m < solution[parent1][k].size(); j++, m++) {///spr czy dobre j
                if (j < numberTasksFromParent) {///spr czy dobre j
                    if (solution[parent1][k][m] == maintenance || solution[parent1][k][m] < 0) {//if it's not a task make iteration again
                        j--;
                    }
                    else {
                        newSolution.push_back(solution[parent1][k][m]);
                        alreadyInNew[solution[parent1][k][m]] = true;
                    }
                }
                else {	//rest of task comes from second parent
                    for (int i = 0; i < problemSize; i++) {//check if new solution has already all tasks
                        if (alreadyInNew[i] == 0) {
                            isThatAll = false;
                            break;
                        }
                        isThatAll = true;
                    }
                    while (!isThatAll) {
                        int first = solution[parent2][k].size();
                        for (int d = 0, m = 0; d < problemSize, m < solution[parent2][k].size(); d++, m++) {
                            if (solution[parent2][k][m] == maintenance || solution[parent2][k][m] < 0) {
                                d--;
                            }
                            else {
                                if (alreadyInNew[solution[parent2][k][m]] == 0) {//writ down 'm' to know which cell of parent2 to take 
                                    if (first > m)
                                        first = m;
                                }
                            }
                        }
                        newSolution.push_back(solution[parent2][k][first]);
                        alreadyInNew[solution[parent2][k][first]] = true;
                        for (int i = 0; i < problemSize; i++) {//check if newSolution has all tasks 
                            if (alreadyInNew[i] == 0) {
                                isThatAll = false;
                                break;
                            }
                            isThatAll = true;
                        }
                    }
                }
            }
            solution[i].push_back(newSolution);
        }
    }
}

void addMutations() {
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < 2; j++) {
            int mutationNumber = rand() % (problemSize / 5);	//random number of mutations but no more than 20% of solution size
            for (int h = 0; h < mutationNumber; h++) {
                int x = rand() % (problemSize - 1);
                int tmp = solution[i][j][x];
                solution[i][j][x] = solution[i][j][x + 1];
                solution[i][j][x + 1] = tmp;
            }
        }
    }
}

void saveToFile() {
    fstream file;
    file.open("lubiePlacki.txt", ios::out);
    int theBestSolution = 0;
    float theBestTime = populationScores[0];	//start with first solution as best score 

    for (int i = 0; i < populationSize; i++) {
        if (populationScores[i] < theBestTime) {
            theBestTime = populationScores[i];
            theBestSolution= i;
        }
    }
    file << "Cmax: " << theBestTime << endl;
    file << "M1: ";
    for (int i = 0; i < solution[theBestSolution][0].size(); i++) {
        if (solution[theBestSolution][0][i] == 30000) {
            file << "m" << breakLen << " ";
        }
        else if (solution[theBestSolution][0][i] > 0) {
            file << solution[theBestSolution][0][i] << " ";
        }
        else {
            file << "idle" << solution[theBestSolution][0][i] * -1 << " ";
        }
    }
    file << endl;
    file << "M2: ";
    for (int i = 0; i < solution[theBestSolution][0].size(); i++) {
        if (solution[theBestSolution][0][i] == 30000) {
            file << "m" << breakLen << " ";
        }
        else if (solution[theBestSolution][0][i] > 0) {
            file << solution[theBestSolution][0][i] << " ";
        }
        else {
            file << "idle" << solution[theBestSolution][0][i] * -1 << " ";
        }
    }
    /*file << endl;
    file << "laczna ilosc przerw na maszynie pierwszej: 0" << endl;
    file << "laczna ilosc przerw na maszynie drugiej: " << maintenanceCount << ", sumaryczny czas: " << maintenanceTime << endl;
    file << "laczna ilosc przerw typu idle na maszynie pierwszej: 0" << endl;
    file << "laczna ilosc przerw typu idle na maszynie pierwszej: " << idleCount << ", sumaryczny czas: " << idleTime << endl;*/

    file.close();
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
