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
        tryCreateTestBlock(15, "small n ", 1, 1e2, "small n");
        tryCreateTestBlock(20, "medium n ", 1e4, 1e5, "medium n");
        tryCreateTestBlock(5, "large n ", 1e7, 1e8, "large n");

        tryCreateTestBlock(10, "repeated numbers small n ", 1e2, 1e3, "repeated numbers small n", 0, 1e2);
        tryCreateTestBlock(10, "repeated numbers medium n ", 1e4, 1e5, "repeated numbers medium n", 0, 1e3);
        
        unsigned int averageTestNumber = 3;

        map<string, timeData> ans;
        if(true)
            ans = testAllTestsInDirectories("input", averageTestNumber, cout);
        else
            ans = testAllTestsInDirectories("input", averageTestNumber);

        timeData res;
        for (auto x : ans) res += x.second;

        cout << string(4, '\n');
        for (auto x : ans)
            cout << x.second.getTestBlock("DIRECTORY : " + x.first) << endl;
        cout << string(4, '\n');
        cout << res.getTestBlock("TOTALE REZE (NOT WEIGHTED)") << endl;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    return 0;
}