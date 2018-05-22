#ifndef CONFIG_H
#define CONFIG_H
#include "myMath.h"
#include <bitset>
#include <iostream>
#include <vector>

//A column in a configuration is represented by a short unsigned integer
using Column = suint;

const uchar MAX_NUM_COLS = 10; //change this if desired. Processing time might be very long with 12 or more. Usually one might want to set it at 8 for speed. I'll leave it as 10 for usability.

const suint TWO_POWER = (1 << suint(MAX_NUM_COLS));

using BColumn = bitset<MAX_NUM_COLS>;

///Main configuration class
class Config
{
private:
    uchar m_iNumrows{};
    
    //The configuration is stored as an array of integers of size 2^Numrows. The index represents the column, the number the number of times that column appears
    vector<uchar> m_Colset;
    
    //the equivalence class is stored as a list of arrays corresponding to row permutation
    mutable vector< vector<uchar> > m_EquivalenceClass; 
    mutable bool m_bNeedsUpdate{}; //Does the equivalence class need update?

    //Do some pre-processing
    static void BakeColumnSums();
    static void BakeBinary();
    static bool m_HasInit;

public:
    explicit Config(suint c, uchar r);
    explicit Config(const vector<VB>& G);
    explicit Config(const vector<uchar>& colset);

    uchar operator[](Column column) const { return m_Colset[column]; }
    uchar& operator[](Column column) { m_bNeedsUpdate = true; return m_Colset[column]; }

    operator const VUC&() const { return m_Colset; }

    Config Simplified() const;

    //Various ways to extract a configuration from the rows of this configuration
    //Returns the configuration whose rows are G[0],G[1],...
    Config ConfigFromRows(const vector<uchar>& G) const;
    
    //Returns a configuration whose rows are those on which G[i] is true. G.size() must be m_iNumrows
    Config ConfigFromRows(const VB& G) const;
    
    //Treating subset written in binary as a set.
    Config ConfigFromRows(suint subset) const;
    
    //Return a configuration 
    Config ConfigOfColsum(uchar n) const;

    //This fills out the equivalence class vector
    void EnsureEquivalenceClassUpdate() const;

    //Removes each row and returns the list.
    vector<Config> MinusRow() const;

    //For finding what is missing.
    VUC ColSumsPresent(suint bigColumnNumRows) const;
    
    //Converts configuration into a {0,1}-matrix
    vector<VB> Table() const;

    //Returns the size of m_Colset, which = 2^m_iNumrows
    nuint AllColumns() const { return m_Colset.size(); }
    uchar NumRows() const { return m_iNumrows; }
    suint NumColumns() const { return SumVI(m_Colset); }
    nuint NumOnes() const;
    
    //prints to the console for sage
    void PrintForSage() const;

    void AddColumn(Column D);
    void RemoveColumn(Column D);
    void ReplaceColumn(Column A, Column D);

    bool IsSubMatrixOf(const Config& other) const;

    const vector< vector<uchar> >& EquivalenceClass() const
    {
        EnsureEquivalenceClassUpdate();
        return m_EquivalenceClass;
    }

    void PrintAsWIM() const;
    
    static bool Init();

    static vector<uchar> m_ColumnSums;
    static vector<BColumn> m_Binary;

    static Config K(uchar n);
    static Config K(uchar n, uchar k);
    static Config I(uchar n);
    static Config T(uchar n);
    static Config Ic(uchar n);
    static Config IIcT(VSUI hMI, VSUI hMIc, VSUI hMT);
    
    vector<Config> InductiveChildren() const;
    
    //helper for predicted by conjecture. info is a vector of integers 0,1,2, representing I, Ic and T. Returns whether or not the configuration is contained in IxIx...xT
    bool subConfOfIIcT(VSUI info) const; 
    
    
    //Friend operators
    //Is it contained but is not equal
    friend bool operator<(const Config& lhs, const Config& rhs);

    //Is lhs subconfiguration of rhs?
    friend bool operator<=(const Config& lhs, const Config& rhs);

    //Is lhs row and column permutation of rhs?
    friend bool operator==(const Config& lhs, const Config& rhs);

    //Just Print
    friend std::ostream& operator<<(std::ostream& os, const Config& rhs);

};

inline Column ToColumn(const VB& G)
{
    Column col = 0;
    for (suint i = 0; i < G.size(); ++i)
    {
        col += static_cast<Column>(G[i])*(1 << i);
    }
    return col;
}

inline Column ToColumn(BColumn G)
{
    Column col = 0;
    for (suint i = 0; i < MAX_NUM_COLS; ++i)
    {
        col += static_cast<int>(G[i])*(1 << i);
    }
    return col;
}

inline Column ColumnOfOnes(suint n) { return (1 << (n)) - 1;}

inline BColumn ToBinary(Column a) { return Config::m_Binary[a]; }

inline VB ToBinaryVB(Column a)
{
    VB hello(MAX_NUM_COLS, false);
//     cout << "toBinaryVB" << endl;
    BColumn bla = ToBinary(a);
//     cout << "bla = " << bla << endl;
    for (suint i = 0; i < MAX_NUM_COLS; ++i)
    {
//         cout << "i = " << i << endl;
        hello[i] = bla[i];
    }
//     cout << "Done with ToBinaryVB" << endl;
    return hello;
}

std::ostream& operator<<(std::ostream& os, BColumn bcol);

VSUI subsetsI(suint n, suint r);

uchar SumVB(BColumn vb);

Column ColumnFromRows(Column col, const vector<uchar>& G);
Column ColumnFromRows(Column col, const VB& G);
Column ColumnFromRows(Column col, suint subset);

inline uchar ColumnSum(Column A) { return Config::m_ColumnSums[A]; }

Config operator|(const Config& lhs, const Config& rhs);
Config operator*(const Config& lhs, const Config& rhs);
Config operator-(const Config& lhs, const Config& rhs);

//Bah
Config EnterConfig();

// void NextConfig(uchar numrows, Config& G);

class ConfigSort
{
public:
    bool operator()(const Config& lhs, const Config& rhs)
    {
        if (lhs.NumRows() < rhs.NumRows())
            return true;
        if (lhs.NumRows() > rhs.NumRows())
            return false;

        if (lhs.NumColumns() < rhs.NumColumns())
            return true;
        if (lhs.NumColumns() > rhs.NumColumns())
            return false;

        if (lhs.NumOnes() < rhs.NumOnes())
            return true;
        if (lhs.NumOnes() > rhs.NumOnes())
            return false;

        return false;
    }
};

#endif // CONFIG_H
