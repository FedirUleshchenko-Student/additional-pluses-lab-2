#pragma once
#include <ratio>
#include <chrono>
#include <string>

// for 1 (seconds):
// using RATIO = std::ratio<1, 1>;

using RATIO = std::milli;
const std::string RATIO_NAME = "ms";

const size_t KSIZ = 15;
const size_t K[] = {1,2,3,4,6,8,10,12,16,20,24,32,64,100,128};

using dataT = unsigned int;
using timeT = long double;