#include "config.h"
#include "multiconfig.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>

vector<uchar> Config::m_ColumnSums(TWO_POWER, 0);
vector<BColumn> Config::m_Binary(TWO_POWER, BColumn(0));

bool Config::m_HasInit = Init();

Config::Config(suint c, uchar r)
    : m_iNumrows(r)
    , m_Colset((1 << nuint(r)), 0)
    ,

    m_bNeedsUpdate(true)
{
    m_Colset[0] = c;
}

Config::Config(const vector<VB>& G)
    : m_iNumrows(G[0].size())
    , m_Colset(1 << G[0].size(), 0)
    ,

    m_bNeedsUpdate(true)
{
    for (const auto& i : G)
    {
        Column w = ToColumn(i);
        ++m_Colset[w];
    }
}

Config::Config(const vector<uchar>& colset)
    : m_iNumrows(log2(colset.size()))
    , m_Colset(colset)
    , m_bNeedsUpdate(true)
{}

void Config::BakeColumnSums()
{
    for (Column col = 0; col < TWO_POWER; ++col)
    {
        m_ColumnSums[col] = uchar(SumVB(ToBinaryVB(col)));
    }
}

list<Config> ReadFromFile(string filename)
{
    ifstream myFile(filename.c_str());
    list<Config> vcread;
    if (!myFile)
    {
        filename = "Configs/" + filename;
        myFile.open(filename.c_str());
        if (!myFile)
        {
            cout << "File not found!" << endl;
            return vcread;
        }
    }
    cout << "Reading file... " << endl;
    char line[255];

    while (myFile)
    {
        vector<VB> Table;
        suint row = 0;
        while (true)
        {
            VB Row;
            for (char& i : line)
                i = 'm';
            myFile.getline(line, 255);

            if (line[1] == 'm')
            {
                break;
            }

            for (char i : line)
            {
                if (i == '0')
                    Row.push_back(0);
                else if (i == '1')
                    Row.push_back(1);
            }
            Table.push_back(Row);

            cout << line << endl;

            ++row;
        }
        cout << "----------" << endl;
        Config A(Transposed(Table));
        vcread.push_back(A);
    }

    cout << "File succesfully read!" << endl;
    myFile.close();
    return vcread;
}

vector<VB> Config::Table() const
{
    vector<VB> toReturn;
    toReturn.reserve(NumColumns());
    for (Column col = 0; col < AllColumns(); ++col)
    {
        for (uchar n = 0; n < m_Colset[col]; ++n)
        {
            VB hello = ToBinaryVB(col);
            hello.resize(NumRows(), 0);
            toReturn.push_back(hello);
        }
    }
    return toReturn;
}

Config Config::Simplified() const
{
    Config A = *this;
    for (Column col = 0; col < AllColumns(); ++col)
    {
        if (A[col] > 1)
            A[col] = 1;
    }

    return A;
}

nuint Config::NumOnes() const
{
    nuint suma = 0;
    for (Column col = 0; col < AllColumns(); ++col)
    {
        suma += nuint(m_Colset[col])*nuint(ColumnSum(col));
    }
    return suma;
}

Column ColumnFromRows(Column col, const vector<uchar>& G)
{
    BColumn original = ToBinary(col);
    BColumn toReturn(0);
    for (suint i = 0; i < G.size(); ++i)
    {
        toReturn[i] = original[G[i]];
    }
    return ToColumn(toReturn);
}

Column ColumnFromRows(Column col, const VB& G)
{
    VB original = ToBinaryVB(col);
    VB toReturn(SumVB(G), 0);
    uchar suma = 0;
    suint m = G.size();
    for (suint i = 0; i < m; ++i)
    {
        toReturn[suma] = original[i];
        if (G[i])
            ++suma;
    }
    return ToColumn(toReturn);
}

Column ColumnFromRows(Column col, suint subset)
{
    if (subset > TWO_POWER)
        return ColumnFromRows(col, ToBinaryVB(subset));

    static vector<VUC> table(TWO_POWER, VUC(TWO_POWER, 0));
    static bool filled = false;

    if (!filled)
    {
        for (Column c = 0; c < TWO_POWER; ++c)
        {
            for (suint ss = 0; ss < TWO_POWER; ++ss)
            {
                table[c][ss] = ColumnFromRows(c, ToBinaryVB(ss));
            }
        }
        filled = true;
    }

    return table[col][subset];
}

VSUI subsetsI(suint n, suint r)
{
    static bool filled = false;
    static vector<VSUI> table(MAX_NUM_COLS + 1, VSUI());

    if (!filled)
    {
        // fill
        for (Column i = 0; i < TWO_POWER; ++i)
        {
            table[ColumnSum(i)].push_back(i);
        }
        filled = true;
    }

    VSUI toReturn;
    toReturn.insert(
      toReturn.end(), table[r].begin(), table[r].begin() + Binomial(n, r));
    return toReturn;
}

Config Config::ConfigFromRows(const vector<uchar>& G) const
{
    Config toReturn(0, G.size());
    for (Column col = 0; col < AllColumns(); ++col)
    {
        if (m_Colset[col] > 0)
        {
            toReturn[ColumnFromRows(col, G)] += m_Colset[col];
        }
    }
    return toReturn;
}

Config Config::ConfigFromRows(const VB& G) const
{
    assert((G.size() == NumRows()) &&
           "Config::ConfigFromRows (G.size() == NumRows())");

    Config toReturn(0, SumVB(G));

    for (Column col = 0; col < AllColumns(); ++col)
    {
        if (m_Colset[col] > 0)
            toReturn[ColumnFromRows(col, G)] += m_Colset[col];
    }

    return toReturn;
}

Config Config::ConfigFromRows(suint subset) const
{
    Config toReturn(0, ColumnSum(subset));

    for (Column col = 0; col < AllColumns(); ++col)
    {
        if (m_Colset[col] > 0)
            toReturn[ColumnFromRows(col, subset)] += m_Colset[col];
    }

    return toReturn;
}

Config Config::ConfigOfColsum(uchar n) const
{
    Config toReturn = *this;
    for (Column col = 0; col < AllColumns(); ++col)
    {
        if (ColumnSum(col) != n)
            toReturn[col] = 0;
    }
    return toReturn;
}

void Config::EnsureEquivalenceClassUpdate() const
{
    if (!m_bNeedsUpdate)
    {
        return;
    }
    list<VUC> permList = PermutationList(NumRows());
    list<VUC>::iterator vit;

    list<VUC> RowPerms;

    // First, construct the list of all permutations of rows.
    for (vit = permList.begin(); vit != permList.end(); ++vit)
    {
        Config D = ConfigFromRows(*vit);
        RowPerms.push_back(D.m_Colset);
    }

    while (!RowPerms.empty())
    {
        VUC D = RowPerms.front();
        RowPerms.pop_front();
        bool shouldAdd = true;
        for (const auto& m_EquivalenceClas : m_EquivalenceClass)
        {
            if (m_EquivalenceClas <= D)
            {
                shouldAdd = false;
                break;
            }
        }
        if (shouldAdd)
        {
            m_EquivalenceClass.push_back(D);
        }
    }
    m_bNeedsUpdate = false;
}

VUC Config::ColSumsPresent(suint bigColumnNumRows) const
{
    VUC G(bigColumnNumRows + 1, 0);
    assert(bigColumnNumRows >= NumRows() &&
           "FATAL ERROR IN Config::ColSumsPresent");

    suint difference = bigColumnNumRows - NumRows();

    for (suint col = 0; col < AllColumns(); ++col)
    {
        for (suint sum = ColumnSum(col); sum <= ColumnSum(col) + difference;
             ++sum)
        {
            G[sum] = max(G[sum], m_Colset[col]);
        }
    }

    return G;
}

void Config::PrintAsWIM() const { PrintForSage(); }

std::ostream& operator<<(std::ostream& os, const Config& rhs)
{

    if (rhs.NumRows() == 0 || rhs.NumColumns() == 0)
    {
        cout << "[]" << endl;
        return os;
    }
    os << endl;

    vector<VB> pretable = rhs.Table();
    vector<VB> table = pretable;
    suint count = 0;
    for (suint sum = 0; sum < pretable[0].size() + 1; ++sum)
    {
        for (const auto& col : pretable)
        {
            if (SumVB(col) == sum)
            {
                table[count] = col;
                ++count;
            }
        }
    }

    if (table.empty())
        return os;
    if (table[0].empty())
        return os;

    for (suint y = 0; y < table[0].size(); ++y)
    {
        os << "[";
        for (auto& x : table)
        {
            os << x[y];
        }
        os << "]" << endl;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, BColumn bcol)
{
    os << "[";
    for (suint i = 0; i < bcol.size(); ++i)
    {
        os << bcol[i];
    }
    os << "]";
    return os;
}

bool Config::Init()
{
    BakeBinary();
    BakeColumnSums();
    return true;
}

void Config::BakeBinary()
{
    for (suint col = 0; col < TWO_POWER; ++col)
    {
        for (suint i = 0; i < MAX_NUM_COLS; ++i)
        {
            Column indicator = (1 << i);
            m_Binary[col][i] = ((col & indicator) == indicator);
        }
    }
}

void Config::PrintForSage() const
{
    vector<VB> table = Table();

    if (table.empty())
        return;
    if (table[0].empty())
        return;

    cout << "Matrix([" << endl;
    for (suint y = 0; y < table[0].size(); ++y)
    {
        cout << "[";
        for (suint x = 0; x < table.size() - 1; ++x)
        {
            cout << table[x][y] << ",";
        }
        cout << table[table.size() - 1][y] << "]";
        if (y != table[0].size() - 1)
            cout << ",";
        cout << endl;
    }
    cout << "])" << endl;
}

Config Config::K(uchar n)
{
    Config KK(0, n);
    for (Column i = 0; i < KK.AllColumns(); ++i)
    {
        KK[i] = 1;
    }
    return KK;
}

Config Config::K(uchar n, uchar k)
{
    Config A(0, n);
    for (Column col = 0; col < A.AllColumns(); ++col)
    {
        if (ColumnSum(col) == k)
            A.AddColumn(col);
    }
    return A;
}

Config Config::I(uchar n)
{
    Config Id(0, n);
    for (suint i = 0; i < n; ++i)
    {
        Id[(1 << i)] = 1;
    }
    return Id;
}

Config Config::Ic(uchar n)
{
    Config Ic(0, n);
    suint twopowerminusone = (1 << n) - 1;
    for (suint i = 0; i < n; ++i)
    {
        suint twopow = (1 << i);
        Ic[(twopowerminusone - twopow)] = 1;
    }
    return Ic;
}

Config Config::T(uchar n)
{
    Config Tr(1, n);
    for (suint i = 1; i < n + 1; ++i)
    {
        Tr[(1 << i) - 1] = 1;
    }
    return Tr;
}

Config Config::IIcT(VSUI hMI, VSUI hMIc, VSUI hMT)
{
    Config partI(0, 0);
    for (unsigned short n : hMI)
    {
        Config Id = I(n);
        Id.AddColumn(0);
        Id.AddColumn(0);
        partI = partI*Id;
    }

    Config partIc(0, 0);
    for (unsigned short n : hMIc)
    {
        Config Ic = Config::Ic(n);
        Column ones = (1 << n) - 1;
        Ic.AddColumn(ones);
        Ic.AddColumn(ones);
        partIc = partIc*Ic;
    }

    Config partT(0, 0);
    for (unsigned short n : hMT)
    {
        Config Tr = Config::T(n);
        Tr = Tr | Tr;
        partT = partT*Tr;
    }

    Config product = partI*partIc*partT;

    return product;
}

bool Config::subConfOfIIcT(VSUI info) const
{

    if (info.size() > NumRows())
        return true;
    VSUI G;
    list<VSUI>
      frontier; // the VSUI represents how many rows for each part of info
    frontier.push_back(G);
    while (!frontier.empty())
    {
        VSUI rowPos[3];
        VSUI F = frontier.back();
        frontier.pop_back();
        if (F.size() > info.size())
            continue;
        suint sumF = SumVI(F);
        if (sumF > NumRows())
            continue;

        bool FhasAZero = false;
        for (unsigned short i : F)
        {
            if (i == 0)
                FhasAZero = true;
        }

        if ((F.size() < info.size()) && (sumF < NumRows()))
        {
            suint t = 0;
            if (FhasAZero)
                t = 1;
            for (; t <= NumRows() - sumF; ++t)
            {
                VSUI W = F;
                W.push_back(t);
                frontier.push_back(W);
            }
            continue;
        }
        if ((sumF != NumRows()) || (F.size() != info.size()))
            continue;

        for (suint i = 0; i < info.size(); ++i)
        {
            rowPos[info[i]].push_back(F[i]);
        }

        // Make a configuration out of the information in rowPos
        Config Big = IIcT(rowPos[0], rowPos[1], rowPos[2]);

        // Repeating a column more than twice makes no sense.
        for (Column col = 0; col < Big.AllColumns(); ++col)
        {
            uchar numtimescolinBig = Big[col];
            if (numtimescolinBig > 2 || (FhasAZero && numtimescolinBig == 1))
                Big[col] = 2;
        }

        if ((*this) <= Big)
        {
            return true;
        }
    }

    return false;
}

vector<Config> Config::InductiveChildren() const
{
    vector<Config> toReturn;

    // add the one with columns halfed.
    Config F = *this;
    for (suint i = 0; i < F.AllColumns(); ++i)
    {
        F[i] = (F[i] + 1)/2;
    }
    toReturn.push_back(F);

    uchar m = NumRows();
    VSUI subs = subsetsI(m, m - 1);
    Config K1 = Config::K(1);
    for (unsigned short S : subs)
    {
        Config H = ConfigFromRows(S);
        for (Column col = 0; col < H.AllColumns(); ++col)
        {
            if (H[col] < 2)
                continue;

            while (*this <= H*K1)
            {
                --H[col];
            }
            ++H[col];
        }
        toReturn.push_back(H);
    }

    return ConvertToVector(Minimals(ConvertToList(toReturn)));
}

void Config::AddColumn(Column D)
{
    ++m_Colset[D];
    m_bNeedsUpdate = true;
}

void Config::ReplaceColumn(Column A, Column D)
{
    m_Colset[D] += m_Colset[A];
    m_Colset[A] = 0;

    m_bNeedsUpdate = true;
}

void Config::RemoveColumn(Column D)
{
    if (m_Colset[D] > 0)
        --m_Colset[D];

    m_bNeedsUpdate = true;
}

vector<Config> Config::MinusRow() const
{
    VB G;
    for (suint i = 0; i < NumRows(); ++i)
        G.push_back(1);

    vector<Config> ret;

    for (suint i = 0; i < NumRows(); ++i)
    {
        G[i] = 0;
        ret.push_back(ConfigFromRows(G));
        G[i] = 1;
    }
    return ret;
}

Config operator|(const Config& lhs, const Config& rhs)
{
    Config suma(0, lhs.NumRows());
    for (Column i = 0; i < lhs.AllColumns(); ++i)
    {
        suma[i] = lhs[i] + rhs[i];
    }
    return suma;
}

Config operator-(const Config& lhs, const Config& rhs)
{
    Config suma(0, lhs.NumRows());
    for (suint i = 0; i < lhs.AllColumns(); ++i)
    {
        if (lhs[i] > rhs[i])
            suma[i] = lhs[i] - rhs[i];
    }
    return suma;
}

Config operator*(const Config& lhs, const Config& rhs)
{
    if (lhs.NumRows() == 0 || lhs.NumColumns() == 0)
        return rhs;

    if (rhs.NumRows() == 0 || lhs.NumColumns() == 0)
        return lhs;
    assert(lhs.NumRows() + rhs.NumRows() <= MAX_NUM_COLS && "Config operator*");
    Config AxB(0, lhs.NumRows() + rhs.NumRows());
    for (Column lcol = 0; lcol < lhs.AllColumns(); ++lcol)
    {
        for (Column rcol = 0; rcol < rhs.AllColumns(); ++rcol)
        {
            if (lhs[lcol] != 0 && rhs[rcol] != 0)
            {
                Column prodcol = lcol + (rcol << suint(lhs.NumRows()));
                AxB[prodcol] += lhs[lcol]*rhs[rcol];
            }
        }
    }
    // cout << "lhs = " << lhs << endl;
    // cout << "rhs = " << rhs << endl;
    // cout << "product = " << AxB << endl;

    return AxB;
}

bool operator<=(const Config& lhs, const Config& rhs)
{
    if (lhs.NumRows() > rhs.NumRows())
        return false;
    if (lhs.NumColumns() > rhs.NumColumns())
        return false;
    //    if (lhs.NumOnes() > rhs.NumOnes()) return false;

    VSUI subListI = subsetsI(rhs.NumRows(), lhs.NumRows());
    for (unsigned short i : subListI)
    {
        Config B = rhs.ConfigFromRows(i);

        for (const auto& i : lhs.EquivalenceClass())
        {
            if (i <= VUC(B))
            {
                return true;
            }
        }
    }
    return false;
}

bool operator<(const Config& lhs, const Config& rhs)
{
    if (lhs.NumRows() == rhs.NumRows() && lhs.NumColumns() == rhs.NumColumns())
        return false;

    return (lhs <= rhs);
}

bool operator==(const Config& lhs, const Config& rhs)
{
    if (lhs.NumRows() != rhs.NumRows())
        return false;

    if (lhs.NumColumns() != rhs.NumColumns())
        return false;

    if (lhs.NumOnes() != rhs.NumOnes())
        return false;

    return (lhs <= rhs);
}

bool Config::IsSubMatrixOf(const Config& other) const
{
    for (Column i = 0; i < AllColumns(); ++i)
    {
        if (m_Colset[i] > other[i])
            return false;
    }
    return true;
}

Config EnterConfig()
{
    suint nr = 200;
    suint nc = 200;
    while (nr > 20)
    {
        cout << "Number of ROWS:   ";
        cin >> nr;
    }

    while (nc > 20)
    {
        cout << "Number of COLUMNS:  ";
        cin >> nc;
    }

    cout << "Please enter a " << nc << "x" << nr
         << " matrix, column by column: " << endl;
    vector<VB> Table(nc, VB(nr, 0));
    for (suint col = 0; col < nc; col++)
    {
        for (suint row = 0; row < nr; row++)
        {
            suint t = 2;
            while (!(t == 0 || t == 1))
            {
                cout << "A[" << col << "," << row << "] = ";
                cin >> t;
            }

            Table[col][row] = bool(t);
        }
        cout << endl;
    }
    Config A(Table);
    cout << "This is the matrix you entered:";
    cout << A;

    return A;
}

uchar SumVB(BColumn vb)
{
    uchar suma = 0;
    for (uchar row = 0; row < MAX_NUM_COLS; ++row)
    {
        suma += static_cast<int>(vb[row]);
    }
    return suma;
}

// void NextConfig(uchar numrows, Config& G)
// {
//     static uchar m = 0;
//     static list<Config> frontier;
//
//     if (m != numrows)
//     {
//         m = numrows;
//         frontier.clear();
//         Config empty(0, m);
//         frontier.push_back(empty);
//     }
//
//     while (!frontier.empty()) {}
// }