#ifndef MYMATH_H
#define MYMATH_H

#include <bitset>
#include <iostream>
#include <list>
#include <set>
#include <vector>

using namespace std;
//using std::bitset;
//using std::vector;

using uchar = std::uint8_t;
using suint = std::uint16_t;
using nuint = std::uint32_t;
using luint = std::uint64_t;

using VUI = vector<nuint>;
using VSUI = vector<suint>;
using VUC = vector<uchar>;

using VUIi = vector<nuint>::iterator;
using VSUIi = vector<suint>::iterator;
using VUCi = vector<uchar>::iterator;

using VB = vector<bool>;

VUI ConvertToVUI(const VSUI& G);
VUI ConvertToVUI(const VUC& G);

VSUI ConvertToVSUI(const VUI& G);
VSUI ConvertToVSUI(const VUC& G);

VUC ConvertToVUC(const VUI& G);
VUC ConvertToVUC(const VSUI& G);

bool operator<=(const VUC& lhs, const VUC& rhs);

list<VUC> PermutationList(suint n, suint k = 0);
//list<VUC> CartesianProduct(suint n, suint k);
list<VUC> AllSubMultiSets(const VUC& G);

const vector<VB>& subsets(nuint n, nuint r);

VSUI RandomPermutation(suint n);
VSUI RandomPermutation(const VSUI& G);
VSUI Identity(suint n);

luint Binomial(luint n, luint r);

VB operator&(const VB& A, const VB& B);
VB operator|(const VB& A, const VB& B);

VUC max(const VUC& lhs, const VUC& rhs);

vector<VB> Transposed(vector<VB> Table);

bool operator<(const VB& A, const VB& B);

nuint SumVI(const VUI& vi);
nuint SumVI(const VUC& vi);
nuint SumVI(const VSUI& vi);
suint SumVB(const VB& vb);

bool NextSubset(VB& G);
VB ToSubset(nuint num);

bool NextMultiset(const VUC& complete, VUC& G);

bool ProbabilityOfTrue(float p);

std::ostream& operator<<(std::ostream& os, const VUI& rhs);
std::ostream& operator<<(std::ostream& os, const VSUI& rhs);
std::ostream& operator<<(std::ostream& os, const VUC& rhs);
std::ostream& operator<<(std::ostream& os, const VB& rhs);

#endif
