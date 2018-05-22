/**
 *Title: Fobidden Configurations
 *Author: Miguel Raggi
 *Contact: mraggi@gmail.com
 *License: MIT
 */
#include "helpers.h"
#include <iostream>

void Initialize()
{
    subsetsI(0,0);
    ColumnFromRows(0,0);
}

void SafeRead(int& x)
{
    string s;
    if (!(cin >> s) || (s.empty()))
    {
        x = 8;
        return;
    }
    try
    {
        x = stoi(s);
    } catch(...)
    {
        x = -1;
        cerr << "Invalid option! Please choose a number between 0 and 8" << endl;
    }
}

int main()
{
//     Initialize();
//     auto F = ConvertToVector(ReadFromFile("F8"));
//     int numRows = 0;
// 
//     for (auto& i : F)
//     {
//         if (numRows < i.NumRows())
//             numRows = i.NumRows();
//     }
//     
//     WhatIsMissing(F,numRows);
//     
//     return 0;
    
    try
    {
        cout << "Welcome to the Forbidden Configuration Program." << endl;
        cout << endl << "PLEASE WAIT... Loading... " << endl;
        cout << "Finished Loading! Choose your option: ";
            
        Initialize();
        
        srand(std::time(nullptr));
        int option = -1;

        do
        {
            cout << "0: What is Missing" << endl;
            cout << "1: What the conjecture predicts" << endl;
            cout << "2: Find Forb (Local)" << endl;
            cout << "3: Find Forb (Exact)" << endl;
            cout << "4: Compare two matrices " << endl;
            cout << "5: Classification by Conjecture" << endl;
            cout << "6: Inductive Children" << endl;
            cout << "7: Other " << endl;
            cout << "8: Exit " << endl;

            cout << "Please choose an option: ";
            SafeRead(option);
            if (option == 8)
            {
                std::cout << "Goodbye!" << std::endl;
                return 0;
            }
            if (option > 8 || option < 0)
            {
                cout << "\nINVALID OPTION!! Please choose a number between 0 and 8\n\n" << endl;
            }
        } while (option > 8 || option < 0);
        
        vector<Config> ForbConf;
        while (ForbConf.empty() && option != 5)
        {
            ForbConf = ConvertToVector(EnterListOfConfigs());
        }

        if (option == 0)
        {
            ExecuteWIM(ForbConf);
        }
        else if (option == 1)
        {
            ExecutePredicted(ForbConf);
        }
        else if (option == 2)
        {
            cout << endl;
            cout << "\t*******************************************" << endl;
            cout << "\t* Welcome to the find forb super-program. *" << endl;
            cout << "\t*******************************************" << endl;
            cout << endl;

            ExecuteFindForb(ForbConf);
        }
        else if (option == 3)
        {
            ExecuteFindExactForb(ForbConf);
        }
        else if (option == 4)
        {
            ExecuteCompare(ForbConf);
        }
        else if (option == 5)
        {
            ExecuteClassify();
        }
        else if (option == 6)
        {
            ExecuteInductiveChildren(ForbConf);
        }
        else if (option == 7)
        {
            cout << "There is no other right now! Goodbye!" << endl;
            option = 8;
            // ExecuteOther(ForbConf);
        }
        else
        {
            cout << "Goodbye!" << endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "An unhandled exception occurred: " << e.what() << "\n";
        return -1;
    }
    catch (...)
    {
        std::cerr << "An unknown exception occurred\n";
        return -1;
    }
    cout << "Program finished successfully." << endl;
    return 0;
}
