#ifndef POPULATION_H
#define POPULATION_H
#include "individual.h"

using vInd = vector<Individual>;
using vIndIter = vector<Individual>::iterator;

//This is a population of individuals. It separates non uniques from uniques in order to save time
class Population
{
private:
    vector<Individual> m_Uniques;
    vector<Individual> m_NonUniques;
    
    bool m_bUniquesNeedSort;
    bool m_bNonUniquesNeedSort;
    nuint m_iMaxPopSize;
    
    nuint m_iBestFitnessUnique;
    nuint m_iBestFitnessNonUnique;
    
public:
    Population(nuint PopulationSize, nuint mps);

    void InsertUnique(const Individual& indy);
    void InsertNonUnique(const Individual& indy);
    
    void Merge(const Population& pop);

    void MakeUnique();
    void KilloffLosers();

    void Clear() { m_Uniques.clear(); m_NonUniques.clear(); }

    nuint Size() const {return m_Uniques.size() + m_NonUniques.size();}
    bool EmptyUniques() const { return m_Uniques.empty(); }
    bool EmptyNonUniques() const { return m_NonUniques.empty(); }
    bool Empty() const { return (EmptyUniques() && EmptyNonUniques()); }

    nuint BestFitness() const { return max(m_iBestFitnessNonUnique, m_iBestFitnessUnique); }
    
    void EnsureSorted();

    Population HillClimbing();
    Population BestIndividuals();
    
    void EvolveStep();
    nuint Evolve(nuint numGenerations);
    
    nuint MaxPopulationSize() { return m_iMaxPopSize; }
    //High probability means more chance of selecting higher-scoring individuals.
    const Individual& SelectRandomWithFitness(double prob = 0.333);
    
    void print(std::ostream& os = std::cout) const;

    ~Population();
private:
    nuint BestFitnessUniques() const;
    nuint BestFitnessNonUniques() const;
    
    void EnsureSortedUniques();
    void EnsureSortedNonUniques();
    
    bool IndividualsWithFitnessUnique(nuint fitness, nuint& first, nuint& last);
    bool IndividualsWithFitnessNonUnique(nuint fitness, nuint& first, nuint& last);
    
    const Individual& SelectRandomWithFitnessUnique(double prob = 0.5);
    const Individual& SelectRandomWithFitnessNonUnique(double prob = 0.5);
    
    nuint NonUniqueSize() const {return m_NonUniques.size();}
    nuint UniqueSize() const {return m_Uniques.size();}
};

std::ostream& operator<<(std::ostream& os, const Population& rhs);

#endif // POPULATION_H
