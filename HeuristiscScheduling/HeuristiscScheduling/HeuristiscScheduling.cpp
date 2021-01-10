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
const int iterationsNumber = 10;   //number of iterations to run
bool iterations = true; // set app to run given iterations number if true, if false app run given perioid of time
const int populationSize = 180; 
const int parentsForGeneration = 10;    //number of parents to be selected fo next generation
std::string input = "in.txt";
//given problem
int problemSize = 10;///przerób tab poniżej na dynamiczny
std::vector<std::vector<int>> problem;    //tasks times
int breakLen = 0;   //break length
int maxTimeBetweenBreaks = 0; //maximum time between two holes
//variables
clock_t start, stop;
int populationScores[populationSize]; //score of each solution (Cmax)
std::vector<std::vector<std::vector<int>>> solutions;  //solutions of current iteration
std::vector<int>parents; //parents of current solutions
std::vector<std::vector<int>> bestSolution;   //the best that has been found

int countCmax(int id) {
    int m2len = solutions[id][1].size();
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
        probabilityOfParent[i] = (1 - (populationScores[i] / sumOfScores)) / sumOfScores;   //as we want the smallest score to have the biggest probability we have to substract from 1...
        probabilitySum += probabilityOfParent[i];   //probabilitest created like that won't sum to 1, so we have to sum them
    }

    for (int i = 0; i < parentsForGeneration; i++) {
        float x = fmod((double)(rand() / 100.0), probabilitySum);   //picking random number from (0,probabilitySum)
        int j = 0;
        probabilitySum = 0;
        while (probabilitySum < x) {
            probabilitySum += probabilityOfParent[j];
            j++;
        }
        if (j == populationSize)j--;    //just in case

        std::vector<int>::iterator it;
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
            std::vector<int>newSolution; //vector for new solution
            std::vector<bool> alreadyInNew;  //write here what you take from praent1, so you know what to take form parent2
            for (int i = 0; i < problemSize; i++) {
                alreadyInNew.push_back(false);
            }

            bool isThatAll = false;
            int numberTasksFromParent = percentageOfParent * problemSize / 100;

            for (int j = 0, m = 0; j < numberTasksFromParent + 1, m < solutions[parent1][k].size(); j++, m++) {///spr czy dobre j
                if (j < numberTasksFromParent) {///spr czy dobre j
                    if (solutions[parent1][k][m] == 30000 || solutions[parent1][k][m] < 0) {//if it's not a task make iteration again
                        j--;
                    }
                    else {
                        newSolution.push_back(solutions[parent1][k][m]);
                        alreadyInNew[solutions[parent1][k][m]] = true;
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
                        int first = solutions[parent2][k].size();
                        for (int d = 0, m = 0; d < problemSize, m < solutions[parent2][k].size(); d++, m++) {
                            if (solutions[parent2][k][m] == 30000 || solutions[parent2][k][m] < 0) {
                                d--;
                            }
                            else {
                                if (alreadyInNew[solutions[parent2][k][m]] == 0) {//writ down 'm' to know which cell of parent2 to take 
                                    if (first > m)
                                        first = m;
                                }
                            }
                        }
                        newSolution.push_back(solutions[parent2][k][first]);
                        alreadyInNew[solutions[parent2][k][first]] = true;
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
            solutions[i].push_back(newSolution);
        }
    }
}

void addMutations() {
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < 2; j++) {
            int mutationNumber = rand() % (problemSize / 5);	//random number of mutations but no more than 20% of solution size
            for (int h = 0; h < mutationNumber; h++) {
                int x = rand() % (problemSize - 1);
                int tmp = solutions[i][j][x];
                solutions[i][j][x] = solutions[i][j][x + 1];
                solutions[i][j][x + 1] = tmp;
            }
        }
    }
}

bool readFile() {
    /// <summary>
    /// problem_size
    /// tasks_on_M1
    /// taska_on_M2
    /// maintanance_len tau(time_without_maintanace)
    /// </summary>
    std::fstream file;
    file.open(input, std::ios::in); //std::string input = "in.txt";
    std::string help= "";

    if (file.is_open())
    {
        getline(file, help);
        problemSize = std::stoi(help);
        for (int i = 0; i < problemSize; i++) {
            problem[0].push_back(0);
            problem[1].push_back(0);
        }

        for (int i = 0; i < problemSize; i++) {
            getline(file, help);
            problem[0][i] = std::stoi(help);
        }

        getline(file, help);

        for (int i = 0; i < problemSize; i++) {
            getline(file, help);
            problem[1][i] = std::stoi(help);
        }

        getline(file, help);

        getline(file, help);
        breakLen = std::stoi(help);

        getline(file, help);
        maxTimeBetweenBreaks = std::stoi(help);

        file.close();
    }
    else {
        return false;
    }
    return true;
}

void saveToFile() {
    std::fstream file;
    file.open("lubiePlacki.txt", std::ios::out);
    int theBestSolution = 0;
    float theBestTime = populationScores[0];	//start with first solution as best score 

    for (int i = 0; i < populationSize; i++) {
        if (populationScores[i] < theBestTime) {
            theBestTime = populationScores[i];
            theBestSolution= i;
        }
    }
    file << "Cmax: " << theBestTime << std::endl;
    file << "M1: ";
    for (int i = 0; i < solutions[theBestSolution][0].size(); i++) {
        if (solutions[theBestSolution][0][i] == 30000) {
            file << "m" << breakLen << " ";
        }
        else if (solutions[theBestSolution][0][i] > 0) {
            file << solutions[theBestSolution][0][i] << " ";
        }
        else {
            file << "idle" << solutions[theBestSolution][0][i] * -1 << " ";
        }
    }
    file << std::endl;
    file << "M2: ";
    for (int i = 0; i < solutions[theBestSolution][0].size(); i++) {
        if (solutions[theBestSolution][0][i] == 30000) {
            file << "m" << breakLen << " ";
        }
        else if (solutions[theBestSolution][0][i] > 0) {
            file << solutions[theBestSolution][0][i] << " ";
        }
        else {
            file << "idle" << solutions[theBestSolution][0][i] * -1 << " ";
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
    std::cout << "WELCOME IN SCHEDULING PROGRAM, HOPE THAT OUR HEURISTIC WILL FIND SATYSFAING SOLUTIONS FOR YOU\n";
    bool timeIsOver = false;        //tells if iterations are done or time have gone
    if (readFile()) {
        start = clock();
        if (iterations) {
            //debugMePlz();
            for (int i = 0; i < populationSize; i++) {
                populationScores[i] = countCmax(i);
            }
            while (timeIsOver < iterationsNumber) {
                roulette();
                nextGen();
                addMutations();
                //debugMePlz();
                for (int i = 0; i < populationSize; i++) {
                    populationScores[i] = countCmax(i);
                }
                timeIsOver++;
            }
            stop = clock();
        }
        else {
            //debugMePlz();
            for (int i = 0; i < populationSize; i++) {
                populationScores[i] = countCmax(i);
            }
            while (!timeIsOver) {
                roulette();
                nextGen();
                addMutations();
                //debugMePlz();
                for (int i = 0; i < populationSize; i++) {
                    populationScores[i] = countCmax(i);
                }
                stop = clock();
                if (workingTime <= ((double)(stop - start) / CLOCKS_PER_SEC)) timeIsOver = true;
            }
        }

        saveToFile();
    }
    else {
        std::cout << "troubles opening the file\n";
    }

    system("Pause");
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
