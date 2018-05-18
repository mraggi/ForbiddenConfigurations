#include "individual.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <utility>

Config Individual::Complete(Config::K(2));
vector<Config> Individual::m_F;

Individual::Individual()
    : m_VSUI(RandomPermutation(Complete.NumColumns()))
    ,

    m_Config(0, 0)
{
    SeparateColumns();
    assert(m_VSUI.size() == Complete.NumColumns());
}

Individual::Individual(VSUI vi)
    : m_VSUI(std::move(vi)), m_iBreakPoint(0), m_Config(0, 0)
{
    SeparateColumns();
    assert(m_VSUI.size() == Complete.NumColumns());
}

Individual::Individual(nuint fitness) : m_iBreakPoint(fitness), m_Config(0, 0)
{}

Individual::Individual(const Individual& father, const Individual& mother)
    : m_VSUI(father.m_VSUI)
    , m_iBreakPoint(father.m_iBreakPoint)
    , m_Config(father.m_Config)
{
    nuint mombreak = rand()%mother.Fitness();
    nuint lastindex = 0;
    for (nuint g = 0; g < m_VSUI.size(); ++g)
    {
        for (nuint b = 0; b < mombreak; ++b)
        {
            if (father.m_VSUI[g] == mother.m_VSUI[b])
            {
                m_VSUI[g] = mother.m_VSUI[lastindex];
                ++lastindex;
            }
        }
    }
    SeparateColumns();
    MutateALittle();
}

void Individual::SeparateColumns()
{
    Config Best(0, Individual::Complete.NumRows());
    VSUI good;
    VSUI bad;
    good.reserve(m_VSUI.size());
    bad.reserve(m_VSUI.size());
    // cout << "============================ STARTING SEPARATE COLUMNS for this
    // nrows: " << suint(Individual::Complete.NumRows()) << endl; cout << "for "
    // << m_VSUI << endl;
    suint firstBadOne = BinarySearch(0, m_VSUI.size());
    // cout << "firstBadOne = " << firstBadOne << endl;
    for (suint i = 0; i < firstBadOne; ++i)
    {
        Best.AddColumn(m_VSUI[i]);
        good.push_back(m_VSUI[i]);
    }
    // cout << "Best = " << Best << endl;

    VSUI remainingcols(m_VSUI.begin() + (firstBadOne), m_VSUI.end());
    // cout << "remaining cols = " << remainingcols << endl;

    for (unsigned short col : remainingcols)
    {
        // cout << "adding col = " << col << endl;
        Config Blah = Best;
        Blah.AddColumn(col);
        bool someonecontainsm_F = false;
        suint m_Fsize = Individual::m_F.size();
        for (suint j = 0; j < m_Fsize; ++j)
        {
            if ((Individual::m_F)[j] <= Blah)
            {
                someonecontainsm_F = true;
                break;
            }
        }

        if (someonecontainsm_F)
        {
            // cout << "not added " << col << endl;
            bad.push_back(col);
        }
        else
        {
            // cout << "added " << col << endl;
            good.push_back(col);
            Best = Blah;
        }
    }
    // cout << "good = " << good << endl;
    // cout << "bad = " << bad << endl;
    suint gs = good.size();

    m_VSUI = good;
    m_VSUI.insert(m_VSUI.end(), bad.begin(), bad.end());

    m_Config = Best;
    m_iBreakPoint = gs;
    // cout << "And m_VSUI = " << m_VSUI << endl;
}

suint Individual::BinarySearch(suint start, suint end)
{
    if (end == start)
    {
        return end;
    }

    suint midpoint = (start + end + 1)/2;

    Config small(0, Individual::Complete.NumRows());
    for (suint i = 0; i < midpoint; ++i)
    {
        small.AddColumn(m_VSUI[i]);
    }

    bool smallContainsmF = false;
    for (const auto& i : m_F)
    {
        if (i <= small)
        {
            smallContainsmF = true;
            break;
        }
    }

    if (smallContainsmF)
    {
        if (midpoint - start == 1)
            return start;
        return BinarySearch(0, midpoint);
    }

    return BinarySearch(midpoint, end);
}

Individual Individual::BestNeighbour() const
{
    if (m_VSUI.size() == Fitness())
        return *this;

    Individual BestFound = *this;

    VSUI good(m_VSUI.begin(), m_VSUI.begin() + Fitness());

    for (nuint b = Fitness(); b < m_VSUI.size(); ++b)
    {
        VSUI myGood = RandomPermutation(good);
        VSUI perm;
        perm.reserve(m_VSUI.size());
        perm.push_back(m_VSUI[b]);
        perm.insert(perm.end(), myGood.begin(), myGood.end());
        perm.insert(perm.end(), m_VSUI.begin() + Fitness(), m_VSUI.begin() + b);
        perm.insert(perm.end(), m_VSUI.begin() + b + 1, m_VSUI.end());

        Individual neigh(perm);

        if (neigh.Fitness() > BestFound.Fitness())
        {
            BestFound = neigh;
        }
        else if (neigh.Fitness() == Fitness())
        {
            if (ProbabilityOfTrue(0.4))
            {
                BestFound = neigh;
            }
        }
    }

    return BestFound;
}

Individual Individual::HillClimbing(bool printoutput) const
{
    nuint numtimesfitnessrepeated = 0;
    Individual CurrentBest = *this;
    while (numtimesfitnessrepeated < 15)
    {
        Individual neigh = CurrentBest.BestNeighbour();
        if (neigh.Fitness() == CurrentBest.Fitness())
        {
            ++numtimesfitnessrepeated;
        }
        else if (neigh.Fitness() > CurrentBest.Fitness())
        {
            numtimesfitnessrepeated = 0;
            CurrentBest = neigh;
            if (printoutput)
                cout << "Used HILL CLIMBING to improve to " << neigh.Fitness()
                     << endl;
        }
        else if (neigh.Fitness() < CurrentBest.Fitness())
        {
            cout << "ERROR in Individual::HillClimbing" << endl;
            break;
        }
    }

    return CurrentBest;
}

void Individual::MutateALittle()
{
    int r = rand()%100;
    if (r > 90 && (m_VSUI.size() != m_iBreakPoint))
    {
        nuint first = rand()%m_iBreakPoint;
        nuint second =
          (rand()%(m_VSUI.size() - m_iBreakPoint)) + m_iBreakPoint;
        nuint temp = m_VSUI[first];
        m_VSUI[first] = m_VSUI[second];
        m_VSUI[second] = temp;

        SeparateColumns();
    }
}
