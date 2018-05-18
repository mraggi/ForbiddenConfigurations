#include "population.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>

Population::Population(nuint PopulationSize, nuint mps)
    :

    m_bUniquesNeedSort(true)
    , m_bNonUniquesNeedSort(true)
    , m_iMaxPopSize(mps)
    , m_iBestFitnessUnique(0)
    , m_iBestFitnessNonUnique(0)
{
    m_Uniques.reserve(m_iMaxPopSize + 8);
    m_NonUniques.reserve(m_iMaxPopSize + 8);
    for (nuint i = m_NonUniques.size(); i < PopulationSize; ++i)
    {
        InsertNonUnique(Individual());
    }
}

void Population::InsertUnique(const Individual& indy)
{
    nuint fitness = indy.Fitness();

    if (m_iBestFitnessUnique < fitness)
    {
        m_iBestFitnessUnique = fitness;
        m_Uniques.push_back(indy);
        m_bUniquesNeedSort = true;
        return;
    }

    bool shouldadd = true;
    for (auto& m_Unique : m_Uniques)
    {
        if (Config(m_Unique) == indy)
        {
            shouldadd = false;
            break;
        }
    }

    if (shouldadd)
    {
        m_Uniques.push_back(indy);
        m_bUniquesNeedSort = true;
    }
}

void Population::InsertNonUnique(const Individual& indy)
{
    nuint fitness = indy.Fitness();
    if (m_iBestFitnessNonUnique < fitness)
        m_iBestFitnessNonUnique = fitness;
    m_NonUniques.push_back(indy);
    m_bNonUniquesNeedSort = true;
}

bool Population::IndividualsWithFitnessUnique(nuint fitness,
                                              nuint& first,
                                              nuint& last)
{
    EnsureSortedUniques();
    pair<vIndIter, vIndIter> range =
      equal_range(m_Uniques.begin(), m_Uniques.end(), Individual(fitness));

    if (range.first == range.second)
    {
        return false;
    }

    first = range.first - m_Uniques.begin();
    last = range.second - m_Uniques.begin();

    return true;
}

bool Population::IndividualsWithFitnessNonUnique(nuint fitness,
                                                 nuint& first,
                                                 nuint& last)
{
    EnsureSortedNonUniques();
    pair<vIndIter, vIndIter> range = equal_range(
      m_NonUniques.begin(), m_NonUniques.end(), Individual(fitness));

    if (range.first == range.second)
    {
        return false;
    }

    first = range.first - m_NonUniques.begin();
    last = range.second - m_NonUniques.begin();

    return true;
}

const Individual& Population::SelectRandomWithFitness(double prob)
{
    assert(!Empty() && "Population::SelectRandomWithFitness");

    if ((ProbabilityOfTrue(prob) && !EmptyUniques()) || EmptyNonUniques())
        return SelectRandomWithFitnessUnique(prob);

    return SelectRandomWithFitnessNonUnique(prob);
}

const Individual& Population::SelectRandomWithFitnessUnique(double prob)
{
    EnsureSortedUniques();

    assert(!m_Uniques.empty() && "Population::SelectRandomWithFitnessUnique");

    for (nuint fit = BestFitness(); fit != 0; --fit)
    {
        if (ProbabilityOfTrue(prob))
        {
            nuint first = 0;
            nuint last = 0;
            if (IndividualsWithFitnessUnique(fit, first, last))
            {
                nuint r = rand()%(last - first) + first;

                //                 cout << "fitness = " << fit;
                //                 cout << ", first = " << first;
                //                 cout << ", last = " << last;
                //                 cout << ", size = " << Size();
                //                 cout << " **** selecting r = " << r << endl;
                return m_Uniques[r];
            }
        }
    }

    return m_Uniques[0];
}

const Individual& Population::SelectRandomWithFitnessNonUnique(double prob)
{
    EnsureSortedNonUniques();

    assert(!m_NonUniques.empty() &&
           "Population::SelectRandomWithFitnessNonUnique");

    for (nuint fit = BestFitness(); fit != 0; --fit)
    {
        if (ProbabilityOfTrue(prob))
        {
            nuint first = 0;
            nuint last = 0;
            if (IndividualsWithFitnessNonUnique(fit, first, last))
            {
                nuint r = rand()%(last - first) + first;

                //                 cout << "fitness = " << fit;
                //                 cout << ", first = " << first;
                //                 cout << ", last = " << last;
                //                 cout << ", size = " << Size();
                //                 cout << " **** selecting r = " << r << endl;
                return m_NonUniques[r];
            }
        }
    }

    return m_NonUniques[0];
}

void Population::Merge(const Population& pop)
{
    m_iBestFitnessNonUnique =
      max(m_iBestFitnessNonUnique, pop.m_iBestFitnessNonUnique);
    m_iBestFitnessUnique = max(m_iBestFitnessUnique, pop.m_iBestFitnessUnique);

    for (const auto& m_Unique : pop.m_Uniques)
    {
        bool alreadyhere = false;
        for (auto& i : m_Uniques)
        {
            if (Config(i) == Config(m_Unique))
            {
                alreadyhere = true;
                break;
            }
        }
        if (!alreadyhere)
        {
            m_Uniques.push_back(m_Unique);
        }
    }

    m_NonUniques.insert(
      m_NonUniques.end(), pop.m_NonUniques.begin(), pop.m_NonUniques.end());

    if (!pop.EmptyUniques())
        m_bUniquesNeedSort = true;
    if (!pop.EmptyNonUniques())
        m_bNonUniquesNeedSort = true;
}

void Population::KilloffLosers()
{
    nuint first = 0;
    nuint last = 0;

    // For UNIQUES
    EnsureSortedUniques();
    if (IndividualsWithFitnessUnique(BestFitness(), first, last))
    {
        m_Uniques.erase(m_Uniques.begin() + last, m_Uniques.end());
    }
    else
    {
        m_Uniques.clear();
    }

    // For NON UNIQUES
    EnsureSortedNonUniques();
    if (IndividualsWithFitnessNonUnique(BestFitness(), first, last))
    {
        m_NonUniques.erase(m_NonUniques.begin() + last, m_NonUniques.end());
    }
    else
    {
        m_NonUniques.clear();
    }
}

void Population::MakeUnique()
{
    m_Uniques.reserve(Size());

    while (!m_NonUniques.empty())
    {
        Individual toAdd = m_NonUniques.back();
        m_NonUniques.pop_back();
        bool hasthisbeenaddedalready = false;
        for (auto& m_Unique : m_Uniques)
        {
            if (Config(m_Unique) == Config(toAdd))
            {
                hasthisbeenaddedalready = true;
                break;
            }
        }
        if (!hasthisbeenaddedalready)
        {
            m_Uniques.push_back(toAdd);
        }
    }
}

void Population::EnsureSorted()
{
    EnsureSortedUniques();
    EnsureSortedNonUniques();
}

void Population::EnsureSortedUniques()
{
    if (m_bUniquesNeedSort)
    {
        sort(m_Uniques.begin(), m_Uniques.end());
        m_bUniquesNeedSort = false;
    }
}

void Population::EnsureSortedNonUniques()
{
    if (m_bNonUniquesNeedSort)
    {
        sort(m_NonUniques.begin(), m_NonUniques.end());
        m_bNonUniquesNeedSort = false;
    }
}

Population Population::BestIndividuals()
{
    Population pop = *this;

    pop.KilloffLosers();

    return pop;
}

void Population::EvolveStep()
{
    Population Children = BestIndividuals();

    if (Children.NonUniqueSize() > m_iMaxPopSize - 6)
    {
        Children.MakeUnique();
    }

    // Insert a few random individuals. One out of every five generations,
    // insert a local-search-best
    if (ProbabilityOfTrue(0.2))
        Children.InsertNonUnique(Individual().HillClimbing(false));
    for (nuint i = 0; i < 2; ++i)
    {
        Children.InsertNonUnique(Individual());
    }

    do
    {
        Individual father = SelectRandomWithFitness();
        Individual mother = SelectRandomWithFitness();

        Individual juanito(father, mother);
        Individual pedrita(mother, father);

        if (ProbabilityOfTrue(0.1))
            juanito = juanito.HillClimbing(false);

        if (ProbabilityOfTrue(0.1))
            pedrita = pedrita.HillClimbing(false);

        Children.InsertNonUnique(juanito);
        Children.InsertNonUnique(pedrita);
    } while (Children.NonUniqueSize() < m_iMaxPopSize);

    (*this) = Children;
}

nuint Population::Evolve(nuint numGenerations)
{
    nuint generation = 0;
    nuint bestfitnessfound = BestFitness();
    while (generation < numGenerations + 1)
    {
        EvolveStep();
        nuint bestfitness = BestFitness();
        if (bestfitnessfound < bestfitness)
        {
            bestfitnessfound = bestfitness;
            generation = 0;
            cout << "Used GENETIC to improve to " << bestfitness << endl;
        }
        ++generation;
    }
    return bestfitnessfound;
}

Population Population::HillClimbing()
{
    Population toReturn(0, m_iMaxPopSize);
    vector<Individual>::iterator it;
    for (it = m_Uniques.begin(); it != m_Uniques.end(); ++it)
    {
        toReturn.InsertNonUnique(it->HillClimbing());
    }

    for (it = m_NonUniques.begin(); it != m_NonUniques.end(); ++it)
    {
        toReturn.InsertNonUnique(it->HillClimbing());
    }

    return toReturn;
}

void Population::print(std::ostream& os) const
{
    vector<Individual>::const_iterator it;
    bool printuniqnonuniq = (!m_Uniques.empty() && !m_NonUniques.empty());
    if (printuniqnonuniq)
        os << endl << "Uniques: " << endl;
    for (it = m_Uniques.begin(); it != m_Uniques.end(); ++it)
    {
        os << Config(*it) << endl;
    }

    if (printuniqnonuniq)
        os << "NonUniques: " << endl;
    for (it = m_NonUniques.begin(); it != m_NonUniques.end(); ++it)
    {
        os << Config(*it) << endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Population& rhs)
{
    rhs.print(os);
    return os;
}

Population::~Population() = default;
