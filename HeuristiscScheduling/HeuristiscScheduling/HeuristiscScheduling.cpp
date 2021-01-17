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
const int iterationsNumber = 3;   //number of iterations to run
bool iterations = true; // set app to run given iterations number if true, if false app run given perioid of time
const int populationSize = 10; 
const int parentsForGeneration = 5;    //number of parents to be selected fo next generation
std::string input = "test_4_0.txt";
//given problem
int problemSize = 10;///przerób tab poniżej na dynamiczny
std::vector<std::vector<int>> problem;    //tasks times
int breakLen1 = 0;   //break length
int maxTimeBetweenBreaks1 = 0; //maximum time between two holes
int breakLen2 = 0;   //break length
int maxTimeBetweenBreaks2 = 0; //maximum time between two holes
//variables
clock_t start, stop;
int populationScores[populationSize]; //score of each solution (Cmax)
std::vector<std::vector<std::vector<int>>> solutions;  //solutions of current iteration
std::vector<int>parents; //parents of current solutions
std::vector<std::vector<int>> bestSolution;   //the best that has been found

void populate() {
    //srand(time(NULL));
    std::vector<int>helper;
    for (int m = 0; m < problemSize; m++) {
        helper.push_back(m);//dodanie kolejnych zadań do helpera
    }
    for (int i = 0; i < populationSize; i++) {
        std::vector<std::vector<int>>container;

        random_shuffle(helper.begin(), helper.end());
        container.push_back(helper);//M1
        container.push_back(helper);//M2
        
        solutions.push_back(container);
    }
}

int countCmax(int id) {
    int m2len = solutions[id][1].size();
    int cmax = 0;

    for (int i = 0; i < m2len; i++) {
        if (solutions[id][1][i] == 30000) {//30 000 break
            cmax += breakLen2;
        }
        else if (solutions[id][1][i] >= 0) {//<0,29999> number of taks
            cmax += problem[1][solutions[id][1][i]];
        }
        else {//<-29999,-1>waiting time
            cmax += -1 * solutions[id][1][i];
        }
    }

    std::cout << id << ' ' << cmax << '\n';

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
        probabilityOfParent[i] = (1 - (populationScores[i] / sumOfScores));   //as we want the smallest score to have the biggest probability we have to substract from 1...
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
        solutions[i].erase(solutions[i].begin(), solutions[i].begin() + 2);
    }
}

void addMutations() {
    if (problemSize < 5) {
        return; //nie ma sensu na tak małym rozwiązaniu robić mutacji
    }
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

void applySchedulingRulesM1() {
    int mainten = 30000;
    int timeToBreakM1 = maxTimeBetweenBreaks1;
    std::vector<int>helper;
    for (int s = 0; s < populationSize; s++) {
        helper.clear();
        timeToBreakM1 = maxTimeBetweenBreaks1;
        for (int i = 0; i < problemSize; i++) {
            if (timeToBreakM1 - problem[0][solutions[s][0][i]] < 0) {
                helper.push_back(mainten);
                timeToBreakM1 = maxTimeBetweenBreaks1;
            }
            timeToBreakM1 = timeToBreakM1 - problem[0][solutions[s][0][i]];
            helper.push_back(solutions[s][0][i]);
        }
        solutions[s][0].clear();
        for (int i = 0; i < helper.size(); i++) {
            solutions[s][0].push_back(helper[i]);
        }
    }
}

void applySchedulingRulesM2() {
    int mainten = 30000;//oznaczenie wymuszonej przerwy

    for (int s = 0; s < populationSize; s++) {
        int time1m = 0, time2m = 0, timeToBreakM2 = maxTimeBetweenBreaks2;
        std::vector<int> readyTasksForM2;
        std::vector<int> endedonM1;//czasy zakończeń zadań na M1
        bool isThatAll = false;
        
        for (int i = 0; i < problemSize; i++) {
            endedonM1.push_back(0);
        }

        std::vector<int> helper;//przechowuje M2, bo na razie tylko solution dla M1 jest zapisane i później się skopiuje vector do solM2
        for (int i = 0, j = 0; i < solutions[s][0].size(); i++, j++) {
            //dodanie do czasu M1 kolejnej pozycji
            if (solutions[s][0][i] < 0) {
                time1m += -1 * solutions[s][0][i];
            }
            else if (solutions[s][0][i] >= 0 && solutions[s][0][i] < mainten) {
                time1m += problem[0][solutions[s][0][i]];
                endedonM1[solutions[s][0][i]] = time1m;//przypisz czas zakończenia zadania na M1
                readyTasksForM2.push_back(solutions[s][0][i]);
            }
            else {
                time1m += breakLen1;
            }

            if (i == 0) {
                if (problem[0][solutions[s][0][0]] >= breakLen2) {//zadanie na M1 jest dłuższe bądź równe breakM2 wyzeruj czas na przerwę:
                    //dodaj przerwę najpóźniej jak się da, pozostały czas uzupełnij czasem oczekiwania (może byc i,m,m,m,m dlatego \|/ )
                    if (problem[0][solutions[s][0][0]] % breakLen2) {//jeśli idle nie będzie zerem to go wstaws
                        helper.push_back((problem[0][solutions[s][0][0]] % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                        time2m += -1 * helper[helper.size() - 1];//dadaj czas idle
                        timeToBreakM2 -= solutions[s][0][0];
                    }
                    //resztę uzupełnij przerwami
                    while (problem[0][solutions[s][0][0]] != time2m) {
                        if (mainten == 13) {
                            std::cout << helper[0] << solutions[s][0][0] << time1m << time2m << timeToBreakM2 << readyTasksForM2[0];
                        }
                        helper.push_back(mainten);
                        time2m += breakLen2;
                        timeToBreakM2 = maxTimeBetweenBreaks2;
                    }
                }
                else if(problem[0][solutions[s][0][0]] + problem[1][solutions[s][0][0]] > maxTimeBetweenBreaks2){//jeśli przerwa jest dłuższa niż zad na M1, ale idle+zad na M2
                    //nie zdążą się wykonać przed przerwą to wstaw przerwę na początek, bo nie ma innego wyjścia
                    helper.push_back(mainten);
                    time2m += breakLen2;
                    timeToBreakM2 = maxTimeBetweenBreaks2;
                }
                else {
                    helper.push_back(problem[0][solutions[s][0][0]] * -1);//wsadź na początek idle
                    time2m += problem[0][solutions[s][0][0]];//dpdaj czas idle
                    timeToBreakM2 -= problem[0][solutions[s][0][0]];//odejmin czas idle od czasu do wymaganej konserwacji
                }
                //dodaj zadanie na M2
                helper.push_back(solutions[s][0][0]);
                time2m += problem[1][solutions[s][0][0]];
                timeToBreakM2 -= problem[1][solutions[s][0][0]];
                readyTasksForM2.pop_back();//wiadomo, że jest jedno, więc nei potrzeba używać delet
            }
            else if (readyTasksForM2.size()) {///SPR  jeśli da się dodac jakies zadanie do M2, żeby inaczej nei dodać za dużo źle przerw
                //jeśli sprawdzanie maszyny 1 bardzo wyprzedza sprawdzanie 2 to zrównaj mierwszą odpowiednio
                
                if (time1m > time2m) {
                    std::sort(readyTasksForM2.begin(), readyTasksForM2.end());//posortowane rosnąco
                    for (int t = readyTasksForM2.size() - 1; t >= 0; t--) {
                        if (readyTasksForM2.size() && timeToBreakM2 < problem[1][readyTasksForM2[0]]) {//jesi coś jeszcze zostało, ale nawet najmniejszy się już nie zmiesci
                            if ((timeToBreakM2 - breakLen2) % breakLen2 > 0) {//jeśli idle > 0
                                helper.push_back(((timeToBreakM2 - breakLen2) % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                                time2m += timeToBreakM2 - breakLen2;//dadaj czas idle
                                timeToBreakM2 -= timeToBreakM2 - breakLen2;
                            }
                            helper.push_back(mainten);
                            time2m += breakLen2;
                            timeToBreakM2 = maxTimeBetweenBreaks2;
                        }
                        if (problem[1][readyTasksForM2[t]] <= timeToBreakM2 && time2m >= endedonM1[readyTasksForM2[t]]) {//jeśli problem zmieści się przed przerwą i skończył się na M1
                            helper.push_back(readyTasksForM2[t]);
                            time2m += problem[1][readyTasksForM2[t]];
                            timeToBreakM2 -= problem[1][readyTasksForM2[t]];
                            readyTasksForM2.pop_back();//usuń element, bo juz ododany
                        }
                        else if (readyTasksForM2.size() && timeToBreakM2 >= problem[1][readyTasksForM2[0]]){//jeśłi coś zostało, zmieści się do końca przerwy, ale nie zostało
                            //wstawione to znaczy, że nei skończyło się na M1 i trzeba M2 uzupełnić idlami i przerwami
                            int smallestEndedTime = time1m;//jak przypiszę czas cąłej maszyny to cos powinno byc mniejsze
                            for (int n = 0; n < problemSize; n++) {
                                if (endedonM1[n] > time2m && smallestEndedTime > endedonM1[n]) {
                                    smallestEndedTime = endedonM1[n];
                                }
                            }
                            if ((smallestEndedTime - time2m) % breakLen2 > 0) {//jeśli idle > 0
                                helper.push_back(((smallestEndedTime - time2m) % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                                timeToBreakM2 -= (smallestEndedTime - time2m) % breakLen2;
                                time2m += (smallestEndedTime - time2m) % breakLen2;//dadaj czas idle
                            }
                            //resztę uzupełnij przerwami jeśli trzeba
                            while (smallestEndedTime != time2m) {
                                helper.push_back(mainten);
                                time2m += breakLen2;
                                timeToBreakM2 = maxTimeBetweenBreaks2;
                            }
                        }
                        if (time1m < time2m) {
                            break;//zrównaliśmy sie już z M1, należy sprawdzić co M1 ma dla nas inaczej możemy źle uszeregować
                        }
                    }
                }
            }
            else if(time1m>time2m) {//jeśli nie ma nic do wstawienia ale M1 jest do przodu to wyrównaj za pom idle i maintenance
                if (time1m - time2m >= breakLen2) {
                    if ((time1m - time2m) % breakLen2) {//jeśli idle > 0
                        helper.push_back(((time1m - time2m) % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                        time2m += time1m - time2m;//dadaj czas idle
                        timeToBreakM2 -= time1m - time2m;
                    }
                    //resztę uzupełnij przerwami jeśli trzeba
                    if (time1m > time2m) {
                        while (time1m != time2m) {
                            helper.push_back(mainten);
                            time2m += breakLen2;
                            timeToBreakM2 = maxTimeBetweenBreaks2;
                        }
                    }
                }
                else {
                    helper.push_back((time1m - time2m) * -1);//wsadź na początek idle
                    time2m += time1m - time2m;//dpdaj czas idle
                    timeToBreakM2 -= time1m - time2m;//odejmin czas idle od czasu do wymaganej konserwacji
                }
            }
            if (i == solutions[s][0].size() - 1) {//jeśli to ostatnia iteracja
                if (readyTasksForM2.size()) {
                    isThatAll = false;  //jeśli coś jeszcze do uzupełnienia to ustawisThatAll na false
                }
                else {
                    isThatAll = true;
                }
                while (!isThatAll) {
                    for (int t = readyTasksForM2.size() - 1; t >= 0; t--) {
                        if (readyTasksForM2.size() && timeToBreakM2 < problem[1][readyTasksForM2[0]]) {//jesi coś jeszcze zostało, ale nawet najmniejszy się już nie zmiesci
                            if ((timeToBreakM2 - breakLen2) % breakLen2 > 0) {//jeśli idle > 0
                                helper.push_back(((timeToBreakM2 - breakLen2) % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                                time2m += timeToBreakM2 - breakLen2;//dadaj czas idle
                                timeToBreakM2 -= timeToBreakM2 - breakLen2;
                            }
                            helper.push_back(mainten);
                            time2m += breakLen2;
                            timeToBreakM2 = maxTimeBetweenBreaks2;
                        }
                        if (problem[1][readyTasksForM2[t]] <= timeToBreakM2 && time2m >= endedonM1[readyTasksForM2[t]]) {//jeśli problem zmieści się przed przerwą i skończył się na M1
                            helper.push_back(readyTasksForM2[t]);
                            time2m += problem[1][readyTasksForM2[t]];
                            timeToBreakM2 -= problem[1][readyTasksForM2[t]];
                            readyTasksForM2.pop_back();//usuń element, bo juz ododany
                            if (readyTasksForM2.size()) {
                                isThatAll = false;  //jeśli coś jeszcze do uzupełnienia to ustawisThatAll na false
                            }
                            else {
                                isThatAll = true;
                            }
                        }
                        else if (readyTasksForM2.size() && timeToBreakM2 >= problem[1][readyTasksForM2[0]]) {//jeśłi coś zostało, zmieści się do końca przerwy, ale nie zostało
                            //wstawione to znaczy, że nei skończyło się na M1 i trzeba M2 uzupełnić idlami i przerwami
                            int smallestEndedTime = time1m;//jak przypiszę czas cąłej maszyny to cos powinno byc mniejsze
                            for (int n = 0; n < problemSize; n++) {
                                if (endedonM1[n] > time2m && smallestEndedTime > endedonM1[n]) {
                                    smallestEndedTime = endedonM1[n];
                                }
                            }
                            if ((smallestEndedTime - time2m) % breakLen2 > 0) {//jeśli idle > 0
                                helper.push_back(((smallestEndedTime - time2m) % breakLen2) * -1);//dodaj idle an początku wielkości reszty z dzielenia przez dł przerwy
                                timeToBreakM2 -= (smallestEndedTime - time2m) % breakLen2;
                                time2m += (smallestEndedTime - time2m) % breakLen2;//dadaj czas idle
                            }
                            //resztę uzupełnij przerwami jeśli trzeba
                            while (smallestEndedTime != time2m) {
                                helper.push_back(mainten);
                                time2m += breakLen2;
                                timeToBreakM2 = maxTimeBetweenBreaks2;
                            }
                        }
                    }
                }
            }
        }
        solutions[s][1].clear();
        solutions[s].erase(solutions[s].begin() + 1);//usuwa M2
        solutions[s].push_back(helper);//dodaje nowe M2
    }
}

void printSolutions() {
    for (int i = 0; i < populationSize; i++) {
        std::cout << "solution: " << i << '\n' << "m1: ";

        for (int j = 0; j < solutions[i][0].size(); j++) {
            if (solutions[i][0][j] == 30000) {
                std::cout << "m" << breakLen1 << " ";
            }
            else if (solutions[i][0][j] >= 0) {
                std::cout << solutions[i][0][j] << ": " << problem[0][solutions[i][0][j]]<< " ";
            }
            else {
                std::cout << "idle" << solutions[i][0][j] * -1 << " ";
            }
        }

        std::cout << '\n' << "m2: ";

        for (int j = 0; j < solutions[i][1].size(); j++) {
            if (solutions[i][1][j] == 30000) {
                std::cout << "m" << breakLen2 << " ";
            }
            else if (solutions[i][1][j] >= 0) {
                std::cout << solutions[i][1][j] << ": " << problem[1][solutions[i][1][j]] << " ";
            }
            else {
                std::cout << "idle" << solutions[i][1][j] * -1 << " ";
            }
        }
        std::cout << '\n';
    }
}

bool readFile() {
    /// <summary>
    /// problem_size
    /// tasks_on_M1
    /// tasks_on_M2
    /// maintanance_len tau(time_without_maintanace)
    /// </summary>
    std::fstream file;
    file.open(input, std::ios::in); //std::string input = "in.txt";
    std::string help= "";

    if (file.is_open())
    {
        getline(file, help);
        problemSize = std::stoi(help);
        getline(file, help);//wczytanie pustej
        std::vector<int>helper;
        for (int i = 0; i < problemSize; i++) {
            helper.push_back(0);
        }
        problem.push_back(helper);
        problem.push_back(helper);

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
        breakLen1 = std::stoi(help);

        getline(file, help);
        maxTimeBetweenBreaks1 = std::stoi(help);

        getline(file, help);

        getline(file, help);
        breakLen2 = std::stoi(help);

        getline(file, help);
        maxTimeBetweenBreaks2 = std::stoi(help);

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
            file << "m" << breakLen1 << " ";
        }
        else if (solutions[theBestSolution][0][i] >= 0) {
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
            file << "m" << breakLen2 << " ";
        }
        else if (solutions[theBestSolution][0][i] >= 0) {
            file << solutions[theBestSolution][0][i] << " ";
        }
        else {
            file << "idle" << solutions[theBestSolution][0][i] * -1 << " ";
        }
    }

    file.close();
}

int main()
{
    std::cout << "WELCOME IN SCHEDULING PROGRAM, HOPE THAT OUR HEURISTIC WILL FIND SATYSFAING SOLUTIONS FOR YOU\n";
    bool timeIsOver = false;        //tells if iterations are done or time have gone
    if (readFile()) {
        start = clock();
        if (iterations) {
            populate();
            applySchedulingRulesM1();
            applySchedulingRulesM2();
            printSolutions();
            for (int i = 0; i < populationSize; i++) {
                populationScores[i] = countCmax(i);
            }
            /// <summary>
            /// DODAJ WYBIERANIE NAJLEPSZEGO
            /// </summary>
            /// <returns></returns>
            while (timeIsOver < iterationsNumber) {
                roulette();
                nextGen();
                printSolutions();
                addMutations();
                applySchedulingRulesM1();
                applySchedulingRulesM2();
                printSolutions();
                for (int i = 0; i < populationSize; i++) {
                    populationScores[i] = countCmax(i);
                }
                timeIsOver++;
            }
            stop = clock();
        }
        else {
            populate();
            applySchedulingRulesM1();
            applySchedulingRulesM2();
            for (int i = 0; i < populationSize; i++) {
                populationScores[i] = countCmax(i);
            }
            while (!timeIsOver) {
                roulette();
                nextGen();
                addMutations();
                applySchedulingRulesM1();
                applySchedulingRulesM2();
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
