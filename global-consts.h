#pragma once
#include <ratio>
#include <chrono>
#include <string>

// for 1 (seconds):
// using RATIO = std::ratio<1, 1>;
using RATIO = std::milli;
const std::string RATIO_NAME = "ms";

const size_t KSIZ = 6;
const size_t K[] = {1,2,4,8,16,32};

using dataT = unsigned int;
using timeT = long double;