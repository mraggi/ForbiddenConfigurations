#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include "config.h"
#include "myMath.h"

class Individual
{
private:
    VSUI m_VSUI;
    nuint m_iBreakPoint{0};
    Config m_Config;
public:
    Individual();
    explicit Individual(VSUI  vi);
    
    explicit Individual(nuint fitness); //DUMMY individual. Do NOT use for anything other than fitness!

    Individual(const Individual& father, const Individual& mother);

    nuint Fitness() const { return m_iBreakPoint; }

    void MutateALittle();

    Individual BestNeighbour() const;
    Individual HillClimbing(bool printoutput = true) const;

    operator const Config&() const { return m_Config; }
    operator const VSUI&() const {return m_VSUI; }

    static Config Complete;
    static vector<Config> m_F;

    suint operator[](nuint i) { return m_VSUI[i]; }

private:
    void SeparateColumns();
    suint BinarySearch(suint start, suint end);
};

inline bool operator<(const Individual& lhs, const Individual& rhs)
{
    return lhs.Fitness() > rhs.Fitness();
}

#endif // INDIVIDUAL_H
