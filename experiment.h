#pragma once
#include <string>
#include <filesystem>
#include <functional>
#include <utility>
#include <ostream>

#include "global-consts.h"

struct timeData {
	timeT t_default = 0;
	timeT t_seq = 0;
	timeT t_unseq = 0;
	timeT t_par = 0;
	timeT t_par_unseq = 0;
	timeT t_memory = 0;

	std::vector<timeT> t_my_algorithm = std::vector<timeT>( KSIZ, 0 );

	timeData& operator+=(const timeData& o);
	timeData& operator/=(const timeT& cnt);

	std::string getTestBlock(std::string blockName) const;

	operator std::string() const;
private:
	std::string getVanillaTestMethodString(const timeT &t, const timeT& timeRatiod, std::string&& message) const;
	std::string getOwnTestMethodString(const timeT& timeRatiod) const;
};

std::vector<dataT> readSequence(std::istream& is);

timeT timedTest(std::function<void()> f);

timeData measureSingleTest(std::vector<dataT>&& a, std::vector<dataT>&& b, int times);
timeData measureSingleTestOnce(std::vector<dataT>& a, std::vector<dataT>& b);

timeData testAllTestsInDirectory(std::filesystem::path indir = "", unsigned int testCnt = 1, std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt);

void mergeOwn(std::vector<dataT>& a, std::vector<dataT>& b, std::vector<dataT>& c, size_t threads);