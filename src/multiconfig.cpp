#include "multiconfig.h"
#include <cassert>

/*list<Config> EquivalenceClass(const list<Config>& hello)
{
    list<Config> toReturn;
    list<Config>::const_iterator it;
    for (it = hello.begin(); it != hello.end(); ++it)
    {
        Config DDD = *it;
        list<Config> toAdd = DDD.EquivalenceClass();
        list<Config>::iterator It;
        for (It = toAdd.begin(); It != toAdd.end(); ++It)
        {
            toReturn.push_back(*It);
        }
    }
    return toReturn;
}*/

vector<Config> InductiveChildren(const vector<Config>& ForbConf)
{
    vector<Config> children;
    for (const auto& i : ForbConf)
    {
        vector<Config> indchil = i.InductiveChildren();
        children.insert(children.end(), indchil.begin(), indchil.end());
    }

    return ConvertToVector(Minimals(ConvertToList(children)));
}

Config FindBestComplete(const vector<Config>& L, uchar numrows)
{
    Config bestComplete(0, numrows);
    Config Kn = Config::K(numrows);

    VUC Colsums(numrows + 1, 0);
    for (const auto& i : L)
    {
        Colsums = max(Colsums, i.ColSumsPresent(numrows));
    }

    for (Column col = 0; col < Kn.AllColumns(); ++col)
    {
        uchar sum = ColumnSum(col);

        bestComplete[col] = Colsums[sum];
    }

    return bestComplete;
}

void FindForbExact(vector<Config> ForbConf, nuint m, nuint start)
{
    nuint twopower = (1 << m);

    luint totsize = Binomial(twopower, start);

    if (start == 0)
        start = twopower;

    cout << "Size = " << totsize << endl;
    VB G(twopower, 0);

    for (suint b = 0; b < start; ++b)
        G[b] = 1;

    cout << "Starting with G = " << G << endl;
    luint s = 0;
    nuint lastknownsize = start;
    while (NextSubset(G))
    {
        if ((s%500000 == 0) && (lastknownsize == start))
        {
            cout << s << " done. Percent complete = "
                 << (double(s)/totsize)*100.0;
            lastknownsize = SumVB(G);
            cout << " with size = " << lastknownsize << endl;
        }
        ++s;
        Config Present(0, m);
        for (Column col = 0; col < twopower; ++col)
        {
            if (G[col])
            {
                Present.AddColumn(col);
            }
        }
        bool someonepresent = false;
        for (const auto& j : ForbConf)
        {
            if (j <= Present)
            {
                someonepresent = true;
                break;
            }
        }
        if (!someonepresent)
        {
            cout << "Found! forb(m,{F}) = " << Present.NumColumns() << endl;
            cout << Present << endl;
            break;
        }
    }
}

//*************************** START WHAT IS MISSING
list<Config>
WhatIsMissing(vector<Config> L, suint n, const Config& CustomComplete)
{
    cout << endl << "We are forbidding the following configurations " << endl;
    for (const auto& i : L)
    {
        cout << "=======================";
        cout << i;
        cout << "=======================" << endl;
    }

    if (n == 0)
        n = L[0].NumRows();

    Config Complete(0, n);

    if (CustomComplete.NumRows() == 0 || CustomComplete.NumColumns() == 0)
    {
        Complete = FindBestComplete(L, n);
    }
    else
    {
        Complete = CustomComplete;
    }

    cout << endl << "Complete Matrix is ";
    cout << Complete;
    cout << "---------------------------------------" << endl;

    VUC Sub;
    nuint numtimes = 0;

    nuint totaltimes = 1;
    for (nuint i = 0; i < Complete.AllColumns(); ++i)
    {
        totaltimes *= (Complete[i] + 1);
    }

    list<Config> wim;

    while (NextMultiset(Complete, Sub))
    {
        // cout << endl << "========================================" << endl;
        // cout << "STARTING NUMTIMES = " << numtimes << endl;
        if (numtimes%50000 == 0)
        {
            cout << double(100.0*numtimes)/totaltimes << "% complete!";
            cout << " and WIM size is now " << wim.size() << endl;
        }
        ++numtimes;
        /// We are assuming we are at a new case.
        Config Present(Sub);
        // cout << numtimes << ": Sub =  " << Sub << endl;
        // cout << numtimes << ": Present =  " << Present << endl;

        bool PresentContainsSomeoneInList = false;
        for (const auto& l : L)
        {
            // cout << "l = " << suint(l) << endl;
            if (l <= Present)
            {
                // cout << "finished comparing" << endl;
                PresentContainsSomeoneInList = true;

                // cout << numtimes << ": L[l] =  " << L[l] << endl;

                break;
            }
        }

        if (PresentContainsSomeoneInList)
            continue;

        // cout << "Right before missing!" << endl;

        Config Missing = Complete - Present;

        // cout << numtimes << ": Missing  " << Missing << endl;

        bool MissingContainsSomeoneInWim = false;
        // Test if someone in WIM is contained in Missing. If so, there's no
        // point
        for (auto& Cit : wim)
        {
            if (Cit <= Missing)
            {
                MissingContainsSomeoneInWim = true;
                break;
            }
        }

        if (MissingContainsSomeoneInWim)
            continue;

        // cout << numtimes << ": WIM.size() =  " << wim.size() << endl;
        auto Cit = wim.begin();
        // Test is Missing contains someone in wim. If so, fuck. Just fucking
        // fuck.
        while (Cit != wim.end())
        {
            if (Missing <= (*Cit))
            {
                Cit = wim.erase(Cit);
                continue;
            }
            ++Cit;
        }

        // cout << numtimes << ": pushing back " << endl;

        wim.push_back(Missing);

    } // while next multiset

    cout << "FINISHED with WIM baby, YEAH" << endl;
    cout << "WIM of size = " << wim.size() << endl;

    PrintAsWIM(wim);

    return wim;
}

suint MaxColCount(const vector<Config>& L)
{
    suint mcc = 0;
    for (auto H : L)
    {
        for (Column col = 0; col != H.AllColumns(); ++col)
        {
            if (H[col] > mcc)
                mcc = H[col];
        }
    }
    return mcc;
}

vector<Config> ConvertToVector(const list<Config>& someList)
{
    vector<Config> newvec(someList.begin(), someList.end());
    return newvec;
}

list<Config> ConvertToList(const vector<Config>& someVec)
{
    list<Config> newlist;
    newlist.assign(someVec.begin(), someVec.end());
    return newlist;
}

void PrintAsWIM(const list<Config>& wim)
{
    list<Config>::const_iterator itt;
    suint i = 0;
    for (itt = wim.begin(); itt != wim.end(); ++itt)
    {
        cout << "M" << i << " = ";
        itt->PrintAsWIM();

        ++i;
    }
    cout << endl << endl << "M = [";
    for (suint j = 0; j < i - 1; ++j)
    {
        cout << "M" << j << ", ";
    }
    cout << "M" << i - 1 << "]" << endl;
}

list<Config> EnterListOfConfigs()
{
    list<Config> myList;

    char filename[255] = "none";
    cout << endl
         << "Please write the filename (write \"none\" to enter the matrix): ";
    cin >> filename;

    if (filename != string("none"))
    {
        myList = ReadFromFile(string(filename));

        assert(!myList.empty() && "FATAL ERROR WHILE LOADING FILE: EXITING");
    }
    else
    {
        int numMatrices = 0;
        while ((numMatrices > 20) || (numMatrices == 0))
        {
            cout << "Then how many matrices would you like to enter?  ";
            cin >> numMatrices;
        }

        for (int i = 0; i < numMatrices; i++)
        {
            cout << "Enter new matrix:" << endl;
            Config A = EnterConfig();
            myList.push_back(A);
        }
    }
    cout << "***************************" << endl;
    cout << "* These are the matrices: *" << endl;
    cout << "***************************" << endl;
    list<Config>::iterator it;
    for (it = myList.begin(); it != myList.end(); ++it)
    {
        cout << "--";
        for (suint col = 0; col < it->NumColumns(); ++col)
            cout << "-";
        cout << *it;
        cout << "--";
        for (suint col = 0; col < it->NumColumns(); ++col)
            cout << "-";
        cout << endl;
    }
    cout << "***************************" << endl;
    return myList;
}

//******************************    START PREDICTED BY CONJECTURE
suint PredictedByConjecture(const vector<Config>& configs, bool quiet)
{
    VSUI G;
    list<VSUI> frontier;
    frontier.push_back(G);
    suint maxk = 0;
    suint trying = 0;
    while (!frontier.empty())
    {
        VSUI F = frontier.front();
        frontier.pop_front();
        bool isSub = false;
        vector<Config>::const_iterator it;
        for (it = configs.begin(); it != configs.end(); ++it)
        {
            if (it->subConfOfIIcT(F))
            {
                isSub = true;
                break;
            }
        }
        if (F.size() > trying)
        {
            trying = F.size();
            if (!quiet)
                cout << "========================  " << trying
                     << "  ========================" << endl;
        }

        if (!isSub)
        {
            if (F.size() > maxk)
            {
                if (!quiet)
                    cout << "Best product is now " << F.size() << endl;
                maxk = F.size();
            }
            if (!quiet)
                cout << "F is NOT a configuration of ";

            suint last = 0;
            if (!F.empty())
                last = F.back();
            for (suint t = last; t < 3; ++t)
            {
                VSUI W = F;
                W.push_back(t);
                frontier.push_back(W);
            }
        }
        else
        {
            if (!quiet)
                cout << "F IS a configuration of ";
        }

        if (!quiet)
        {
            for (suint i = 0; i < F.size(); ++i)
            {
                if (F[i] == 0)
                    cout << "I";
                if (F[i] == 1)
                    cout << "Ic";
                if (F[i] == 2)
                    cout << "T";
                if (nuint(i + 1) != F.size())
                    cout << "x";
            }
            cout << endl;
        }
    } // end while
    return maxk;
}

list<Config> Maximals(list<Config> hello)
{
    ConfigSort sorty;
    hello.sort(sorty);
    list<Config> toReturn;
    while (!hello.empty())
    {
        Config F = hello.front();
        hello.pop_front();
        list<Config>::iterator it;
        bool shouldAdd = true;
        for (it = hello.begin(); it != hello.end(); ++it)
        {
            if (F <= *it)
            {
                shouldAdd = false;
                break;
            }
        }
        if (shouldAdd)
        {
            toReturn.push_back(F);
        }
    }
    return toReturn;
}

list<Config> Minimals(list<Config> hello)
{
    ConfigSort sorty;
    hello.sort(sorty);
    list<Config> toReturn;
    while (!hello.empty())
    {
        Config F = hello.back();
        hello.pop_back();
        list<Config>::iterator it;
        bool shouldAdd = true;
        for (it = hello.begin(); it != hello.end(); ++it)
        {
            if (*it <= F)
            {
                shouldAdd = false;
                break;
            }
        }
        if (shouldAdd)
            toReturn.push_back(F);
    }
    return toReturn;
}