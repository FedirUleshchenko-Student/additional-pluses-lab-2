#include "generation.h"
#include <fstream>
#include <random>

using namespace std;

mt19937_64 gen(chrono::system_clock::to_time_t(chrono::system_clock::now()));

vector<dataT> getRandomSequence(size_t size, dataT mn, dataT mx) {
	vector<dataT> ans;
	uniform_int_distribution<dataT> rand(mn, mx);

	while (size > 0) {
		size--;
		ans.push_back(rand(gen));
	}
	sort(ans.begin(), ans.end());
	return ans;
}

void fillTestFile(size_t as, size_t bs, dataT mn, dataT mx, ostream& os) {
	os << as << '\n';
	vector<dataT> sq(getRandomSequence(as, mn, mx));
	for (auto&& x : sq) os << x << " ";
	os << '\n';

	os << bs << '\n';
	sq = getRandomSequence(bs, mn, mx);
	for (auto&& x : sq) os << x << " ";
	os << '\n';
	os.flush();
}

void createTestBlock(size_t testFilesCnt, string namePrefix,
	size_t mnsiz, size_t mxsiz, filesystem::path indir,
	dataT mnval, dataT mxval) {
	filesystem::path dir = filesystem::path("input") / indir;

	if (!filesystem::is_directory(dir))
		filesystem::create_directories(dir);

	uniform_int_distribution<dataT> rand(mnsiz, mxsiz);

	for (int i = 1; i <= testFilesCnt; i++) {
		ofstream fo(format("{}\\{}{}.txt", dir.string(), namePrefix, i));

		fillTestFile(rand(gen), rand(gen), mnval, mxval, fo);
	}
}

void tryCreateTestBlock(size_t testFilesCnt, string namePrefix,
	size_t mnsiz, size_t mxsiz, filesystem::path indir,
	dataT mnval, dataT mxval) {
	filesystem::path dir = filesystem::path("input") / indir;

	if (!filesystem::is_directory(dir))
		createTestBlock(testFilesCnt, namePrefix, mnsiz, mxsiz, indir, mnval, mxval);
}