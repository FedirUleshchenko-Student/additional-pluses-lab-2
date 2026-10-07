#include "generation.h"
#include <fstream>

using namespace std;

mt19937_64 gen(chrono::system_clock::to_time_t(chrono::system_clock::now()));

std::vector<dataT> getRandomSequence(size_t size, dataT mn, dataT mx) {
	vector<dataT> ans;
	uniform_int_distribution<dataT> rand(mn, mx);

	while (size > 0) {
		size--;

		ans.push_back(rand(gen));
	}
	sort(ans.begin(), ans.end());
	return ans;
}

void fillTestFile(size_t as, size_t bs, dataT mn, dataT mx, std::ostream& os) {
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

void createTestBlock(size_t testFilesCnt, std::string namePrefix,
	size_t mnsiz, size_t mxsiz, dataT mnval, dataT mxval, std::filesystem::path dir) {

	if (!filesystem::is_directory(dir))
		filesystem::create_directory(dir);

	uniform_int_distribution<dataT> rand(mnsiz, mxsiz);

	for (int i = 1; i <= testFilesCnt; i++) {
		ofstream fo(format("{}\\{}{}.txt", dir.string(), namePrefix, i));

		fillTestFile(rand(gen), rand(gen), mnval, mxval, fo);
	}
}


//// https://stackoverflow.com/questions/52975114/different-execution-policies-at-runtime
//enum Policy { seq, par, par_unseq };
//template<class F>
//auto maybe_parallel(F f, Policy p) {
//	switch (p) {
//	case seq: return f(std::execution::seq);
//	case par: return f(std::execution::par);
//	default: return f(std::execution::par_unseq);
//	}
//}
//auto f2(const std::vector<std::string>& vec,
//	const std::string& elem, Policy p) {
//	return maybe_parallel
//	([&](auto& pol) {return std::find(pol, vec.begin(), vec.end(), elem); }, p);
//}