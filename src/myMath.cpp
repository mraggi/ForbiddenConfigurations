#include "myMath.h"
#include <cassert>
#include <cstdlib>

luint Binomial(luint n, luint r)
{
    if (r > n - r)
    {
        r = n - r;
    }

    luint numerator = 1;
    luint denominator = 1;

    for (luint i = n; i >= n - r + 1; --i)
        numerator *= i;

    for (luint i = 1; i <= r; ++i)
        denominator *= i;

    return (numerator/denominator);
}

VB operator&(const VB& A, const VB& B)
{
    nuint size = min(A.size(), B.size());
    VB G;
    for (nuint i = 0; i < size; i++)
        G.push_back(A[i] && B[i]);
    return G;
}

VB operator|(const VB& A, const VB& B)
{
    nuint size = min(A.size(), B.size());
    VB G;
    for (nuint i = 0; i < size; i++)
        G.push_back(A[i] || B[i]);
    return G;
}

vector<VB> Transposed(vector<VB> Table)
{
    nuint numrows = Table.size();
    nuint numcols = Table[0].size();
    vector<VB> Transposed;
    for (nuint i = 0; i < numcols; ++i)
    {
        VB row;
        for (nuint j = 0; j < numrows; ++j)
        {
            row.push_back(Table[j][i]);
        }
        Transposed.push_back(row);
    }
    return Transposed;
}

bool operator<(const VB& A, const VB& B)
{
    // cout << "inside < operator for VB!" << endl;
    if (A.size() != B.size())
        return false;
    for (nuint i = 0; i < A.size(); ++i)
        if (A[i] && !B[i])
            return false;
    return true;
}

list<VUC> PermutationList(suint n, suint k)
{
    if (k == 0)
        k = n;

    VUC G;
    list<VUC> frontier;
    frontier.push_back(G);

    list<VUC> permL;

    while (!frontier.empty())
    {
        VUC F = frontier.back();
        frontier.pop_back();
        if (F.size() == k)
        {
            permL.push_back(F);
        }
        else
        {
            for (int t = n - 1; t >= 0; --t)
            {
                bool okToAdd = true;
                for (unsigned char i : F)
                {
                    if (nuint(t) == i)
                    {
                        okToAdd = false;
                        break;
                    }
                }
                if (okToAdd)
                {
                    VUC W = F;
                    W.push_back(t);
                    frontier.push_back(W);
                }

            } // for t
        } // end if not at goal node yet
    } // while frontier > 0
    return permL;
}

VUC Identity(uchar n)
{
    VUC perm(n, 0);
    for (uchar i = 0; i < n; ++i)
        perm[i] = i;
    return perm;
}

VSUI RandomPermutation(suint n)
{
    VSUI perm(n, 0);
    for (suint i = 0; i < n; ++i)
        perm[i] = i;

    for (suint i = 0; i < n; ++i)
    {
        suint j = rand()%(i + 1);
        perm[i] = perm[j];
        perm[j] = i;
    }
    return perm;
}

VSUI RandomPermutation(const VSUI& G)
{
    VSUI W = G;
    suint n = W.size();

    for (suint t = 0; t < n; ++t)
    {
        suint i = rand()%n;
        suint j = rand()%n;
        suint temp = W[i];
        W[i] = W[j];
        W[j] = temp;
    }
    return W;
}
/*
list<VUC> CartesianProduct(nuint n, nuint k)
{
    VUC G;
    list<VUC> frontier;
    frontier.push_back(G);

    list<VUC> cart;

    while (!frontier.empty())
    {
        VUC F = frontier.back();
        frontier.pop_back();
        if (F.size() == k)
        {
            cart.push_back(F);
        } else
        {
            for (int t = n-1; t >= 0; --t)
            {
                VUC W = F;
                W.push_back(t);
                frontier.push_back(W);
            } //for t
        } //end if not at goal node yet
    } // while frontier > 0
    return cart;
}*/

const vector<VB>& subsets(nuint n, nuint r)
{
    const nuint size = 64;
    static vector<VB> SubsetDone(size, VB(size, false));
    static vector<vector<vector<VB>>> SubSets(
      size, vector<vector<VB>>(size, vector<VB>()));

    assert((n < size) && (r < size) && "vector<VB> subsets");

    if (SubsetDone[n][r])
        return SubSets[n][r];

    SubSets[n][r].reserve(Binomial(n, r));

    VB G;
    for (nuint i = 0; i < r; ++i)
        G.push_back(1);
    for (nuint i = r; i < n; ++i)
        G.push_back(0);
    SubSets[n][r].push_back(G);
    bool finished = false;

    while (!finished)
    {
        finished = true;
        bool zeroFound = false;
        int numOnes = 0;
        int subOfWhat = n;
        for (int i = subOfWhat - 1; i > -1; i--)
        {
            if (!G[i])
            {
                zeroFound = true;
                continue;
            }
            if (!zeroFound)
                numOnes++;

            if (zeroFound)
            {
                G[i] = false;
                G[i + 1] = true;
                for (int j = 0; j < numOnes; ++j)
                    G[i + 1 + j + 1] = true;
                for (int j = i + 1 + numOnes + 1; j < subOfWhat; ++j)
                    G[j] = false;
                finished = false;
                break;
            }
        }

        if (!finished)
        {
            SubSets[n][r].push_back(G);
        }
    } // while not finished

    SubsetDone[n][r] = true;

    return SubSets[n][r];
}

VUI ConvertToVUI(const VSUI& G)
{
    VUI toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

VUI ConvertToVUI(const VUC& G)
{
    VUI toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

VSUI ConvertToVSUI(const VUI& G)
{
    VSUI toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

VSUI ConvertToVSUI(const VUC& G)
{
    VSUI toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

VUC ConvertToVUC(const VUI& G)
{
    VUC toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

VUC ConvertToVUC(const VSUI& G)
{
    VUC toReturn(G.size(), 0);
    for (nuint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = G[i];
    }
    return toReturn;
}

nuint SumVI(const VUI& vi)
{
    nuint suma = 0;
    for (unsigned int i : vi)
    {
        suma += i;
    }
    return suma;
}

nuint SumVI(const VUC& vi)
{
    nuint suma = 0;
    for (unsigned char i : vi)
    {
        suma += i;
    }
    return suma;
}

nuint SumVI(const VSUI& vi)
{
    nuint suma = 0;
    for (unsigned short i : vi)
    {
        suma += i;
    }
    return suma;
}

suint SumVB(const VB& vb)
{
    suint suma = 0;
    for (bool i : vb)
    {
        suma += suint(i);
    }
    return suma;
}

bool operator<=(const VUC& lhs, const VUC& rhs)
{
    assert(lhs.size() == rhs.size());

    suint size = lhs.size();
    for (suint i = 0; i < size; ++i)
    {
        if (lhs[i] > rhs[i])
            return false;
    }
    return true;
}

list<VUC> AllSubMultiSets(const VUC& G)
{
    list<VUC> frontier;
    list<VUC> toReturn;
    VUC empty;
    frontier.push_back(empty);

    cout << "Starting AllSubMulti with G = " << G << endl;

    while (!frontier.empty())
    {
        VUC F = frontier.back();
        frontier.pop_back();
        if (F.size() >= G.size())
        {
            cout << "I have a good one! " << F << endl;
            toReturn.push_back(F);
            continue;
        }

        // Add all neighbours
        uchar ofG = G[F.size()];
        for (uchar t = 0; t <= ofG; ++t)
        {
            VUC W = F;
            W.push_back(t);
            frontier.push_back(W);
        } // for t
    }
    cout << "returning from AllSubMultiSets with size = " << toReturn.size()
         << endl;
    return toReturn;
}

bool NextMultiset(const VUC& complete, VUC& G)
{
    static vector<VUC> frontier(1, VUC());
    // cout << "NextMultiSet. frontier size = " << frontier.size() << endl;

    while (!frontier.empty())
    {
        G = frontier.back();
        frontier.pop_back();
        if (G.size() == complete.size())
        {
            return true;
        }
        // Add all neighbours
        uchar ofcompl = complete[G.size()];
        for (uchar t = 0; t <= ofcompl; ++t)
        {
            VUC W = G;
            W.push_back(t);
            frontier.push_back(W);
        } // for t
    }
    return false;
}

bool NextSubset(VB& G)
{
    bool zeroFound = false;
    int numOnes = 0;
    int subOfWhat = G.size();
    for (int i = subOfWhat - 1; i > -1; i--)
    {
        if (!G[i])
        {
            zeroFound = true;
            continue;
        }
        if (!zeroFound)
            numOnes++;

        if (zeroFound)
        {
            G[i] = false;
            G[i + 1] = true;
            for (int j = 0; j < numOnes; j++)
                G[i + 1 + j + 1] = true;
            for (int j = i + 1 + numOnes + 1; j < subOfWhat; j++)
                G[j] = false;
            return true;
        }
    }
    /// If we are at the limit case, all 0 then all 1.
    suint size = 0;
    for (auto&& i : G)
        size += static_cast<int>(i);

    if (size == 0)
        return false;

    if (size > 0)
        size--;
    cout << "size is now = " << size << endl;
    for (int i = 0; i < subOfWhat; ++i)
    {
        if (i < size)
        {
            G[i] = true;
        }
        else
        {
            G[i] = false;
        }
    }
    return true;
}

VUC max(const VUC& lhs, const VUC& rhs)
{
    assert(lhs.size() == rhs.size());

    nuint size = lhs.size();
    VUC toReturn(size, 0);
    for (nuint i = 0; i < size; ++i)
    {
        toReturn[i] = max(lhs[i], rhs[i]);
    }
    return toReturn;
}

VB ToSubset(nuint num, nuint size)
{
    VB subs(size, 0);

    for (nuint i = 0; i < size; ++i)
    {
        subs[i] = (((num >> i)%2) != 0u);
    }
    return subs;
}

bool ProbabilityOfTrue(float p)
{
    double r = double(rand())/RAND_MAX;
    return r < p;
}

std::ostream& operator<<(std::ostream& os, const VUI& rhs)
{
    os << "[ ";
    for (unsigned int rh : rhs)
        os << rh << " ";
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const VUC& rhs)
{
    os << "[ ";
    for (unsigned char rh : rhs)
        os << suint(rh) << " ";
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const VSUI& rhs)
{
    os << "[ ";
    for (unsigned short rh : rhs)
        os << suint(rh) << " ";
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const VB& rhs)
{
    os << "[";
    for (bool rh : rhs)
        os << rh;
    os << "]";
    return os;
}
