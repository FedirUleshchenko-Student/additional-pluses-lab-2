#include "experiment.h"
#include <format>
#include <execution>
#include <algorithm>
#include <vector>
#include <fstream>
#include <thread>
#include "global-consts.h"

#include <iostream>


using namespace std;

timeData& timeData::operator+=(const timeData& o) {
	for (size_t i = 0; i < t_my_algorithm.size(); i++)
		t_my_algorithm[i] += o.t_my_algorithm[i];

	t_default += o.t_default;
	t_seq += o.t_seq;
	t_unseq += o.t_unseq;
	t_par += o.t_par;
	t_par_unseq += o.t_par_unseq;
	t_memory += o.t_memory;

	return *this;
}

timeData& timeData::operator/=(const timeT& cnt){
	for (size_t i = 0; i < t_my_algorithm.size(); i++)
		t_my_algorithm[i] /= cnt;

	t_default /= cnt;
	t_seq /= cnt;
	t_unseq /= cnt;
	t_par /= cnt;
	t_par_unseq /= cnt;
	t_memory /= cnt;

	return *this;
}

string timeData::getTestBlock(string blockName) const {
	return format("{0}\n|{1:^92}|\n{0}\n{2}{0}\n", string(94, '='), blockName, string(*this));
}

string timeData::getOwnTestMethodString(const timeT& timeRatiodTo) const {
	string ans;
	pair<timeT, size_t> mn = { ULLONG_MAX,0 };
	for (size_t i = 0; i < KSIZ; i++) {
		if (mn.first > t_my_algorithm[i] && t_my_algorithm[i] != 0) {
			mn.first = t_my_algorithm[i];
			mn.second = i;
		}
	}

	for (size_t i = 0; i < KSIZ; i++) {
		ans += format("{0:>25} # {1:>17.5f} {4} | {2:>17.5f} {4} | {3:18.7f} |{5}\n",
			format("K = {}", K[i]), t_my_algorithm[i], t_my_algorithm[i] + t_memory, t_my_algorithm[i] / timeRatiodTo, RATIO_NAME,
			(mn.second == i) ? " <-- best on this test block" : "");
	}
	return ans;
}

std::string timeData::getVanillaTestMethodString(const timeT& t, const timeT& timeRatiodTo, string&& message) const {
	return format("{0:>25} # {1:>17.5f} {4} | {2:>17.5f} {4} | {3:18.5f} |\n", message,
		t, t + t_memory, t / timeRatiodTo, RATIO_NAME);
}

timeData::operator string() const {
	return format(
		"{0}{1}{2}{3}{4}{5}{6}{0}{0}{1}{7}{0}",
		string(94, '-') + '\n',
		format("{0:>25} | {1:>15} ({4}) | {2:>15} ({4}) | {3:>15} |\n",
			"test method", "computed time", "total time", "ratio (to default)", RATIO_NAME),
		getVanillaTestMethodString(t_default, t_default, "default time"),
		getVanillaTestMethodString(t_seq, t_default, "seq time"),
		getVanillaTestMethodString(t_unseq, t_default, "unseq time"),
		getVanillaTestMethodString(t_par, t_default, "par time"),
		getVanillaTestMethodString(t_par_unseq, t_default, "par_unseq time"),
		getOwnTestMethodString(t_default)
		);
}



vector<dataT> readSequence(istream& is) {
	size_t siz, x; is >> siz;
	vector<dataT> ans;
	while (siz > 0) {
		siz--;
		is >> x; ans.push_back(x);
	}
	return ans;
}

timeT timedTest(function<void()> f) {
	auto t1 = chrono::steady_clock::now();
	f();
	auto t2 = chrono::steady_clock::now();

	return chrono::duration<timeT, RATIO>(t2 - t1).count();
}

timeData measureSingleTest(vector<dataT>&& a, vector<dataT>&& b, int times) {
	timeData ans;
	for (int i = 0; i < times; i++)
		ans += measureSingleTestOnce(a, b);
	ans /= times;
	return ans;
}

timeData measureSingleTestOnce(vector<dataT>& a, vector<dataT>& b) {
	auto t1 = chrono::steady_clock::now();
	vector<dataT> c(a.size() + b.size());
	timeData ans;
	auto t2 = chrono::steady_clock::now();

	ans.t_memory = chrono::duration<timeT, RATIO>(t2 - t1).count();

	vector<dataT> cTest(a.size() + b.size());
	ans.t_default		= timedTest([&]() {merge(a.begin(), a.end(), b.begin(), b.end(), cTest.begin());});
	ans.t_seq			= timedTest([&]() {merge(execution::seq, a.begin(), a.end(), b.begin(), b.end(), cTest.begin());});
	ans.t_unseq			= timedTest([&]() {merge(execution::unseq, a.begin(), a.end(), b.begin(), b.end(), cTest.begin());});
	ans.t_par			= timedTest([&]() {merge(execution::par, a.begin(), a.end(), b.begin(), b.end(), cTest.begin()); });
	ans.t_par_unseq		= timedTest([&]() {merge(execution::par_unseq, a.begin(), a.end(), b.begin(), b.end(), cTest.begin()); });

	for (size_t t = 0; t < KSIZ; t++) {
		c.clear();
		c.resize(a.size() + b.size());
		ans.t_my_algorithm[t] = timedTest([&, t]() {mergeOwn(a, b, c, K[t]); });
		for (size_t i = 0; i < c.size(); i++) {
			if (c[i] != cTest[i]) {
				cout << "threads : " << K[t] << endl;
				cout << "Needed : "; for (size_t j = 0; j <= i; j++) cout << cTest[i] << " "; cout << endl;
				cout << "Own alg : "; for (size_t j = 0; j <= i; j++) cout << c[i] << " "; cout << endl;
				throw logic_error("OWN ALGORITHM DOES NOT CREATES NEEDED VALUES");
			}
		}
	}
	return ans;
}

timeData testAllTestsInDirectory(filesystem::path indir, unsigned int testCnt, std::optional<std::reference_wrapper<std::ostream>> os) {

	filesystem::path dir = filesystem::path("input" / indir);

	if (!filesystem::is_directory(dir) || filesystem::is_empty(dir))
		throw runtime_error("cannot open the input directory");

	timeData ans;
	timeData t;

	for (const auto x : filesystem::recursive_directory_iterator(dir)) {
		if (filesystem::is_directory(x)) continue;
		ifstream fi(x.path());
		//cout << "opening file = " << x.path() << endl;
		if (!fi.is_open())
			throw runtime_error(format("cannot open input file : {}", x.path().string()));

		t = measureSingleTest(readSequence(fi), readSequence(fi), testCnt);
		if (os.has_value()) os.value().get() << t.getTestBlock(x.path().filename().string());
		ans += t;
	}
	return ans;
}

void mergeOwn(vector<dataT>& a, vector<dataT>& b, vector<dataT>& c, size_t threads) {
	vector<jthread> tr(threads);
	size_t aBlkSiz = a.size() / threads + (a.size() % threads != 0);
	size_t bBlkSiz = b.size() / threads + (b.size() % threads != 0);

	vector<pair<size_t, size_t>> blockLimits(threads);
	for (size_t i = 0; i < threads; i++)
		blockLimits[i] = { i * (aBlkSiz + bBlkSiz) , (i + 1) * (aBlkSiz + bBlkSiz) };
	blockLimits[threads - 1].second = min(blockLimits[threads - 1].second, c.size());

	for (size_t i = 0; i < threads; i++) {
		tr[i] = jthread(
			[&, i]() { merge(
				a.begin() + i * aBlkSiz,
				a.begin() + ((i + 1 == threads) ? a.size() : ((i + 1) * aBlkSiz)),
				b.begin() + i * bBlkSiz,
				b.begin() + ((i + 1 == threads) ? b.size() : ((i + 1) * bBlkSiz)),
				c.begin() + blockLimits[i].first); });
	}

	tr.clear();

	vector<dataT> temp(a.size() + b.size());

	vector<dataT>& fromP = c;
	vector<dataT>& toP = temp;
	size_t lastsiz;
	bool startOffset;
	while (blockLimits.size() > 1) {
		lastsiz = 0;
		startOffset = blockLimits.size() % 2;
		for (size_t i = 0; i < blockLimits.size() - startOffset; i++) {
			merge(fromP.begin() + blockLimits[i].first, fromP.begin() + blockLimits[i].second,
				fromP.begin() + blockLimits[i + 1].first, fromP.begin() + blockLimits[i + 1].second,
				toP.begin() + lastsiz
			);

			blockLimits[i].second = blockLimits[i + 1].second;
			lastsiz += (blockLimits[i].second - blockLimits[i].first);
			blockLimits.erase(blockLimits.begin() + i + 1);
		}

		if (startOffset)
			copy(fromP.begin() + blockLimits.back().first,
				fromP.begin() + blockLimits.back().second,
				toP.begin() + lastsiz);

		swap(fromP, toP);
	}

	if (&fromP != &c)
		swap(temp, c);

	//if(!c.empty())
	//	c.pop_back();
}