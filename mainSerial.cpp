#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

string INPUT = "m50.txt";
float MUTATION_RATE = 0.01;
int POPULATION_SIZE = 1000;
int TIME_LIMIT = 30;

int MUTATIONS_NUM;
int PROCS;
int PROCCESORS;
int CURRENT;
int MINN;

class GA
{
public:
    vector<int> ORDER;
    int LAST_MAX;
    int CURR;
    int CURR_SUM;
};

int fitness(vector<int> &tab, int max, int fit_proc)
{
    int x, s = 0;
    for (x = 0; x < tab.size(); x++)
    {
        if (s + tab[x] <= max)
            s += tab[x];
        else
        {
            s = tab[x];
            fit_proc--;
            if (fit_proc <= 0)
                break;
        }
    }
    return tab.size() - x;
}

int fitnessHelper(vector<int> &tab, int r)
{
    int s = 0;
    for (int x = tab.size() - r; x < tab.size(); x++)
        s += tab[x];
    return s;
}

void shuffle(vector<int> &tab)
{
    for (int i = 0; i < tab.size(); ++i)
    {
        int j = rand() % tab.size();
        swap(tab[i], tab[j]);
    }
}

void doMutate(vector<int> &tab)
{
    for (int i = 0; i < MUTATIONS_NUM; i++)
    {
        int a = rand() % tab.size();
        int b = rand() % tab.size();
        swap(tab[a], tab[b]);
    }
}

int doFitness(GA *g, int max)
{
    if (g->LAST_MAX != max)
    {
        g->LAST_MAX = max;
        g->CURR = fitness(g->ORDER, max, PROCCESORS);
        g->CURR_SUM = fitnessHelper(g->ORDER, g->CURR);
    }
    return g->CURR;
}

void mutate(GA &g)
{
    g.LAST_MAX = 0;
    doMutate(g.ORDER);
}

bool compare(GA &a, GA &b)
{
    doFitness(&a, CURRENT);
    doFitness(&b, CURRENT);
    return a.CURR < b.CURR || (a.CURR == b.CURR && a.CURR_SUM < b.CURR_SUM);
}

int findBest(GA &a, int r)
{
    int l = MINN - 1;
    int m = (l + r) / 2;
    while (m > l)
    {
        doFitness(&a, m) == 0 ? r = m : l = m;
        m = (l + r) / 2;
    }
    return r;
}

int runBest(GA &a)
{
    int max = MINN * 2;
    while (doFitness(&a, max) != 0)
        max *= 2;
    return findBest(a, max);
}

vector<GA> nextGen(vector<GA> &pop)
{
    sort(pop.begin(), pop.end(), compare);
    vector<GA> next;
    next.push_back(pop[0]);
    for (auto &p : pop)
        if (doFitness(&p, CURRENT) == 0)
            next.push_back(p);
    while (next.size() < pop.size()) // mutacja osobników
    {
        GA g = pop[rand() % (rand() % pop.size() + 1)];
        mutate(g);
        next.push_back(g);
    }
    if (doFitness(&next[0], CURRENT) == 0)
        CURRENT = findBest(next[0], CURRENT) - 1;
    return next;
}

vector<int> getData()
{
    vector<int> tab;
    int temp;
    fstream file;
    file.open(INPUT, fstream::in);
    if (!file.good())
        cerr << "File error";
    file >> PROCCESORS >> PROCS;
    while (file >> temp)
        tab.push_back(temp), MINN += temp;
    MINN = MINN / PROCCESORS - MINN % PROCCESORS;
    return tab;
}

int main()
{
    int x = 9999999999;
    srand(time(nullptr));
    auto tab = getData();                              // pobranie danych z pliku
    MUTATIONS_NUM = (int)(MUTATION_RATE * tab.size()); // ustalenie liczby mutaacji na osobnika
    vector<GA> POPULATION;
    int iterations = 0;
    for (int x = 0; x < POPULATION_SIZE; x++) // tworzenie początkowej populacji
    {
        GA g;
        g.LAST_MAX = 0;
        g.ORDER = tab;
        shuffle(g.ORDER);
        POPULATION.push_back(g);
    }
    CURRENT = runBest(POPULATION[0]);
    while ((clock() / CLOCKS_PER_SEC) < TIME_LIMIT) // pilnowanie czasu
    {
        POPULATION = nextGen(POPULATION); // tworzenie nowej generacji
        if (x > CURRENT)
        {
            // cout << clock() / CLOCKS_PER_SEC << "--" << CURRENT << endl;
            x = CURRENT;
        }
        iterations++; // podbicie iteracji
    }
    cout << "Wynik koncowy: " << CURRENT << endl;
    cout << "Liczba iteracji: " << iterations << endl;
    return 0;
}