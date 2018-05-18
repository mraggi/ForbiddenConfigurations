/**
 *Title: Fobidden Configurations
 *Author: Miguel Raggi
 *Contact: mraggi@gmail.com
 *License: GPL v3
 */
#include "helpers.h"
#include <iostream>

int main()
{
    try
    {
        Config K2 = Config::K(2);
        Config K1 = Config::K(1);
        Config K3 = K2*K1;

        srand(std::time(nullptr));
        int option = -1;

        while (option > 8 || option < 0)
        {
            cout << "Welcome to the Forbidden Configuration Program." << endl;
            cout << "0: What is Missing" << endl;
            cout << "1: What the conjecture predicts" << endl;
            cout << "2: Find Forb (Local)" << endl;
            cout << "3: Find Forb (Exact)" << endl;
            cout << "4: Compare two matrices " << endl;
            cout << "5: Classification by Conjecture" << endl;
            cout << "6: Inductive Children" << endl;
            cout << "7: Other " << endl;
            cout << "8: Exit " << endl;

            cout << endl << "PLEASE WAIT... Loading... " << endl;
            K2 <= K3;
            cout << "Finished Loading! Choose your option: ";

            cin >> option;
        }
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
