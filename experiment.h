#pragma once
#include <string>
#include <filesystem>
#include <functional>

#include "global-consts.h"

struct timeData {
	timeT t_default = 0;
	timeT t_seq = 0;
	timeT t_unseq = 0;
	timeT t_par = 0;
	timeT t_par_unseq = 0;

	timeT t_my_straight = 0;
	timeT t_my_smart = 0;

	timeData& operator+=(const timeData& o);

	std::string getAnsBlock(std::string message) const;

	operator std::string() const;
};

std::vector<dataT> readSequence(std::istream& is);

timeT timedTest(std::function<void()> f);

timeData measureSingleTest(std::vector<dataT>&& a, std::vector<dataT>&& b);

std::string testAllTestsInDirectory(std::filesystem::path dir = "input");
