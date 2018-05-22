#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>

#include "multiconfig.h"
#include "population.h"

using namespace std;

//Executes the What is Missing sub-program. Don't use for more than 5 rows!!
inline void ExecuteWIM(vector<Config> ForbConf)
{
    cout << endl;
    cout << "\t*************************************************" << endl;
    cout << "\t* Welcome to the what is missing super-program. *" << endl;
    cout << "\t*************************************************" << endl;
    cout << endl;

    int numRows = 0;

    for (auto & i : ForbConf)
    {
        if (numRows < i.NumRows())
            numRows = i.NumRows();
    }
//////////////////////////////////////////////////
    int def = 0;

    cout << endl;
    cout << "Would you like to customize the options?" << endl;
    cout << "0 = No (*)" << endl;
    cout << "1 = Yes" << endl;
    cout << "Select an option: ";
    cin >> def;

    if (def == 0)
    {
        WhatIsMissing(ForbConf, numRows);
        return;
    }

    int nr = 4;
    cout << endl << "How many rows would you like? (default is " << numRows << ")" << endl;
    cin >> nr;

    if (nr == 0)
        nr = numRows;

    int YN = 0;
    cout << endl;
    cout << "Would you like to enter a custom complete matrix? " << endl;
    cout << "0 = No (*)" << endl;
    cout << "1 = Yes" << endl;
    cout << "Select an option: ";

    cin >> YN;

    Config Complete(0, 0);

    if (YN != 0)
    {
        Complete = EnterConfig();
    }

    WhatIsMissing(ForbConf, nr, Complete);

}

//Find out what the conjecture predicts. Don'e use for more than 8 rows!
inline void ExecutePredicted(const vector<Config>& ForbConf)
{
    cout << endl;
    cout << "\t******************************************************" << endl;
    cout << "\t* Welcome to the conjecture predictor super-program. *" << endl;
    cout << "\t******************************************************" << endl;
    cout << endl;
    nuint i = PredictedByConjecture(ForbConf);
    cout << "___________________________________________________________" << endl << endl;
    cout << "The conjecture predicts forb(m,F) is O(m^" << i << ")"<< endl;
}

//Compare
inline void ExecuteCompare(vector<Config> Blah)
{
    cout << endl;
    cout << "\t**********************************************************" << endl;
    cout << "\t* Welcome to the configuration comparator super-program. *" << endl;
    cout << "\t**********************************************************" << endl;
    cout << endl;
    for(nuint i = 0; i < Blah.size(); ++i)
    {
        for (nuint j = 0; j < Blah.size(); ++j)
        {
            if (i == j)
                continue;
            cout << "Is configuration " << i << " contained in configuration " << j << "? ";
            if (Blah[i] <= Blah[j])
            {
                cout << "Yes!" << endl;
            } else
            {
                cout << "No!" << endl;
            }
        }
    }
}

inline void DoBrute(Population& POP, nuint numtimes)
{
    nuint bestfitness = POP.BestFitness();
    cout << "Doing BRUTE FORCE Search, starting with fitness = " << bestfitness << endl;
    for (nuint i = 0; i < numtimes; ++i)
    {
        Individual bst; //random individual
        if (bst.Fitness() > bestfitness)
        {
            POP.Clear();
            POP.InsertNonUnique(bst);
            bestfitness = bst.Fitness();
            cout << "Used BRUTE FORCE to improve to " << bestfitness << endl;
        } else if (bst.Fitness() == bestfitness && POP.Size() < POP.MaxPopulationSize())
        {
            POP.InsertNonUnique(bst);
        }
    }
    cout << "Best for BRUTE FORCE = " << bestfitness << endl;
    POP.KilloffLosers(); //this should do nothing!
    POP.MakeUnique();
}

inline void DoHillClimbing(Population& POP)
{
    nuint bestfitness = POP.BestFitness();
    cout << "Doing HILL CLIMBING starting with fitness = " << bestfitness << endl;
    Population loc = POP.HillClimbing();
    cout << "Finished. Making list unique..." << endl;
    loc.KilloffLosers();
    loc.MakeUnique();
    bestfitness = loc.BestFitness();
    
    cout << "Best for HILL CLIMBING = " << bestfitness << endl;
    
    POP.Merge(loc);
    POP.MakeUnique();
}

inline void DoGenetic(Population& POP, nuint maxPopSize, nuint numGenerations)
{
    nuint bestfitness = POP.BestFitness();
    for (nuint i = POP.Size(); i < maxPopSize; ++i)
    {
        Individual RandIndi;
        POP.InsertNonUnique(RandIndi);
    }

    cout << "Doing GENETIC Search, starting with fitness = " << POP.BestFitness() << endl;
    POP.Evolve(numGenerations);
    POP.KilloffLosers();

    cout << "Restarting GENETIC with another population... " << endl;
    Population Hello(maxPopSize, maxPopSize);
    Hello.Evolve(numGenerations);
    Hello.KilloffLosers();

    cout << "Combining two populations ... " << endl;
    POP.Merge(Hello);
    
    bestfitness = POP.Evolve(numGenerations);

    cout << "Best for GENETIC = " << bestfitness << endl;
    POP.KilloffLosers();
    POP.MakeUnique();
}

//Find what forb is
inline void ExecuteFindForb(vector<Config> ForbConf)
{
    cout << "Executing FindForb for the following matrices:" << endl;
    for (const auto & i : ForbConf)
    {
        cout << i << endl;
    }
    
    cout << "Up to how many rows would you like to calculate for? ";
    nuint numr = 7;
    cin >> numr;

    const nuint start = 3;
    const nuint maxPopSize = 40;
    const nuint numGenerations = 200;

    //cin >> numr;

    cout << "Would you like to use Brute Force Search? [0/1] : ";
    nuint do_brute = 1;
    //cin >> do_brute;
    cout << endl;

    cout << "Would you like to use Hill Climbing? [0/1] : ";
    nuint do_local = 1;
    //cin >> do_local;
    cout << endl;
    
    cout << "Would you like to use Genetic Algorithms? [0/1] : ";
    nuint do_genetic = 1;
    //cin >> do_genetic;
    cout << endl;


    vector<nuint> results;

    Individual::m_F = ForbConf;

    for (nuint row = start; row < numr+1; ++row)
    {
        cout<<"________________________________________________________"<<endl;

        cout << "\t*******************************" << endl;
        cout << "\t* Starting search for row = " << row << " *" << endl;
        cout << "\t*******************************" << endl;

        Individual::Complete = Config::K(row);

        Population POP(1, maxPopSize);

        if (do_brute != 0u)
        {
            DoBrute(POP, maxPopSize*numGenerations/2);
            cout << "------------------------------------" << endl;
        }

        if (do_local != 0u)
        {
            DoHillClimbing(POP);
            cout << "------------------------------------" << endl;
        }

        if (do_genetic != 0u)
        {
            DoGenetic(POP, maxPopSize, numGenerations);
            cout << "------------------------------------" << endl;
        }
        
        if (do_local != 0u)
        {
            DoHillClimbing(POP);
            cout << "------------------------------------" << endl;
        }
        nuint bestfitness = POP.BestFitness();
        
        results.push_back(bestfitness);
        cout << "Finished this row. Concluded that ";
        cout << "forb(" << row << ") = " << bestfitness;
        cout << POP;
    }

    for (nuint row = start; row < numr+1; ++row)
    {
        cout << "forb("<< row << ", F) = " << results[row-start] << endl;
    }
}

inline list<VSUI> TwoColumns(suint m, suint mincolsum = 0, suint maxcolsum = 0)
{
    if (maxcolsum == 0)
        maxcolsum = m;
    list<VSUI> toReturn;
    for (suint a = 0; a < m; ++a)
    {
        for (suint b = 0; b+a < m+1; ++b)
        {
            if (a + b < mincolsum || a+b > maxcolsum)
                continue;
            for (suint c = b; c < m-a-b+1; ++c)
            {
                if (a+c < mincolsum || a+c > maxcolsum)
                    continue;
                    
                Column col1 = ColumnOfOnes(a+b);
                Column col2 = ColumnOfOnes(a+b+c) - ColumnOfOnes(a+b) + ColumnOfOnes(a);
                VSUI F;
                F.push_back(col1);
                F.push_back(col2);
                toReturn.push_back(F);
            }
        }
    }
    list<VSUI>::iterator it;
    for (it = toReturn.begin(); it != toReturn.end(); ++it)
    {
        cout << "f = " << *it << endl;
    }
    return toReturn;
}

inline void ExecuteClassify()
{
    suint m = 6;
    suint k = 2;
    cout << "How many rows? "; cin >> m;
    cout << "The conjecture predicts O(m^k): what should k be? "; cin >> k;
    
    cout << "Trying to find " << m << "-rowed boundary F's for which forb(m,F) is O(m^" << k << ") simple m-rowed configurations" << endl;
    suint allcols = (1 << suint(m));
    VSUI AcceptedCols;
    //AcceptedCols.reserve();
    for (Column col = 0; col < allcols; ++col)
    {
        if (ColumnSum(col) <= k+1 && m-ColumnSum(col) <= k+1)
            AcceptedCols.push_back(col);
    }
    
    vector<Config> Best;
    list<VSUI> frontier = TwoColumns(m,m-k-1,k+1);

    cout << "Starting while loop" << endl;
    while (!frontier.empty())
    {
        VSUI F = frontier.back();
        frontier.pop_back();
        
        Config A(0,m);
        A.AddColumn(F[0]);
        A.AddColumn(F[1]);
        for (suint i = 2; i < F.size(); ++i)
        {
            A.AddColumn(AcceptedCols[F[i]]);
        }
        
        suint pbc = 0;
        if (!F.empty())
        {
            vector<Config> temp;
            temp.push_back(A);
            pbc = PredictedByConjecture(temp,true);
        }
        /*cout << "F = " << F << endl;
        cout << "A = " << A << endl;
        cout << "Predicted By Conjecture = " << pbc << endl;
        cout << "----------------------------------------------" << endl;*/
        if (pbc < k+1)
        {
            if(pbc == k)
                Best.push_back(A);
            suint t = 0;
            if (F.size() > 2)
                t = F.back();
            if (F.size() > 3 && F[F.size()-2] == t)
                ++t;
            for (; t < AcceptedCols.size(); ++t)
            {
                VSUI W = F;
                W.push_back(t);
                frontier.push_back(W);
            }
        }
    }
    for (auto AcceptedCol : AcceptedCols)
    {
        Config A(0,m);
        A.AddColumn(AcceptedCol);
        vector<Config> tmp;
        tmp.push_back(A);
        if (PredictedByConjecture(tmp,true) == k)
            Best.push_back(A);
    }
    cout << "Finished! Converting to maximals!" << endl;
    vector<Config> maxes = ConvertToVector(Maximals(ConvertToList(Best)));
    cout << "Finished! Converting to minimals!" << endl;
    vector<Config> minis = ConvertToVector(Minimals(ConvertToList(Best)));
    cout << "The minimal cases are:" << endl;
    for (const auto & mini : minis)
    {
        cout << mini << endl;
    }
    cout << "The maximal cases are:" << endl;
    for (const auto & maxe : maxes)
    {
        cout << maxe << endl;
    }
}

inline void ExecuteInductiveChildren(const vector<Config>& ForbConf)
{
    cout << "Welcome to the inductive children super-program" << endl;
    vector<Config> indch = InductiveChildren(ForbConf);
    for (suint i = 0; i < indch.size(); ++i)
    {
        cout << "H" << i << " = " << indch[i] << endl;
    }
}

// inline void ExecuteOther(const vector<Config>& ForbConf)
// {
    //Config A = (ForbConf[0]*ForbConf[1])*ForbConf[2];

    //cout << "A = " << A << endl;
// }

inline void ExecuteFindExactForb(vector<Config> ForbConf)
{
    cout << "How many rows would you like to calculate for?" << endl;
    nuint m = 0;
    cin >> m;
    
    cout << "What size would you like to start at? ";
    nuint n = 0;
    cin >> n;

    FindForbExact(std::move(ForbConf), m, n);
    
}