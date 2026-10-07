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
        //createTestBlock(250, "", 1, 1000, 1, 1000);

        auto t1 = chrono::steady_clock::now();
        cout << testAllTestsInDirectory() << endl;
        auto t2 = chrono::steady_clock::now();
        cout << scientific << chrono::duration<long double, milli>(t2 - t1) << endl;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    return 0;
}