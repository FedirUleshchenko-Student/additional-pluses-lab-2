// Working in Visual Studio 2022, so compiler^
// MSVC (Microsoft Visual C++)
//
// Standard (for version of C++):
// ISO C++20 Standard (/std:c++20)

#include <stdexcept>
#include <iostream>
#include "generation.h"
#include "experiment.h"

using namespace std;

int main()
{
    try {
        //createTestBlock(50, "", 1e7, 1e7, 1, 1e10, "n = 1e7");
        //cout << "tests created" << endl;


        auto t1 = chrono::steady_clock::now();
        auto ans = testAllTestsInDirectory("n = 1e7", 20, cout);
        cout << ans.getTestBlock("TOTALE") << endl;
        auto t2 = chrono::steady_clock::now();
        cout << scientific << chrono::duration<long double, milli>(t2 - t1) << endl;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    return 0;
}