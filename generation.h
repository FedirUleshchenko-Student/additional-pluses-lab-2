#pragma once
#include <random>
#include <vector>
#include <filesystem>

#include "global-consts.h"

std::vector<dataT> getRandomSequence(size_t size, dataT mn, dataT mx);

void fillTestFile(size_t as, size_t bs, dataT mn, dataT mx, std::ostream& os);

void createTestBlock(size_t testFilesCnt, std::string namePrefix = "",
	size_t mnsiz = WINT_MIN, size_t mxsiz = UINT_MAX,
	dataT mnval = WINT_MIN, dataT mxval = UINT_MAX,
	std::filesystem::path dir = "input");