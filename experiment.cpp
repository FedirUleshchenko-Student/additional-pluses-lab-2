#include "experiment.h"
#include <format>
#include <execution>
#include <algorithm>
#include <vector>
#include <fstream>
#include <thread>
#include "global-consts.h"

using namespace std;

timeData& timeData::operator+=(const timeData& o) {
	t_default += o.t_default;
	t_seq += o.t_seq;
	t_unseq += o.t_unseq;
	t_par += o.t_par;
	t_par_unseq += o.t_par_unseq;
	t_my_straight += o.t_my_straight;
	t_my_smart += o.t_my_smart;

	return *this;
}

string timeData::getAnsBlock(string message) const {
	return format("{0}\n{1:^50}\n{0}\n{2}\n{0}\n\n\n", string(50, '='), message, string(*this));
}

timeData::operator string() const {
	return format(
		"{0}\n{1}\n{2}\n{3}\n{4}\n{5}\n{6}\n{7}\n{0}", string(50, '-'),
		format("{0:>20} = {1:<e} {2}", "default time", t_default, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "seq time", t_seq, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "unseq time", t_unseq, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "par time", t_par, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "par_unseq time", t_par_unseq, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "my_straight time", t_my_straight, RATIO_NAME),
		format("{0:>20} = {1:<e} {2}", "my_smart time", t_my_smart, RATIO_NAME)
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

#include <iostream>

timeData measureSingleTest(vector<dataT>&& a, vector<dataT>&& b) {
	vector<dataT> c(a.size() + b.size());
	timeData ans;
	ans.t_default	= timedTest([a, b, &c]() {merge(a.begin(), a.end(), b.begin(), b.end(), c.begin()); });
	ans.t_seq		= timedTest([a, b, &c]() {merge(execution::seq, a.begin(), a.end(), b.begin(), b.end(), c.begin());});
	ans.t_unseq		= timedTest([a, b, &c]() {merge(execution::unseq, a.begin(), a.end(), b.begin(), b.end(), c.begin());});
	ans.t_par		= timedTest([a, b, &c]() {merge(execution::par, a.begin(), a.end(), b.begin(), b.end(), c.begin());});
	ans.t_par_unseq = timedTest([a, b, &c]() {merge(execution::par_unseq, a.begin(), a.end(), b.begin(), b.end(), c.begin());});
	// my tests
	vector<dataT> c2(a.size() + b.size());
	//ans.t_my_straight = timedTest([a, b, &c]() {mergeStraight(a.begin(), a.end(), b.begin(), b.end(), c.begin()); });
	//ans.t_my_smart	  = timedTest([a, b, &c]() {mergeSmart(a.begin(), a.end(), b.begin(), b.end(), c.begin()); });

	return ans;
}

string testAllTestsInDirectory(filesystem::path dir) {
	if (!filesystem::is_directory(dir) || filesystem::is_empty(dir))
		throw runtime_error("cannot open the input directory");

	timeData ans;

	for (const auto x : filesystem::directory_iterator(dir)) {
		ifstream fi(x.path());
		if (!fi.is_open())
			throw runtime_error(format("cannot open input file : {}", x.path().string()));

		auto t1 = chrono::steady_clock::now();
		ans += measureSingleTest(readSequence(fi), readSequence(fi));
		auto t2 = chrono::steady_clock::now();
		cout << scientific << "outside = " << chrono::duration<long double, RATIO>(t2 - t1) << endl;
	}
	return ans.getAnsBlock("TOTALE");
}

/*
vector<dataT>::iterator mergeStraight(
	vector<dataT>::const_iterator first1, vector<dataT>::const_iterator last1,
	vector<dataT>::const_iterator first2, vector<dataT>::const_iterator last2,
	vector<dataT>::iterator d_first) {

	jthread tr[K];

	size_t cnt = (last1 - first1) / K;
	for (size_t i = 1; i <= cnt * K; i++) {
		upper_bound(first2, last2, *(first1 + K - 1));
		auto t = upper_bound(first2, last2, *(first1 + K - 1));
		
		tr[i - 1] = jthread(merge(first1, first1+K-1, first2, t, d_first));

		d_first += K + (t-first2);
		first1 += K;
	}

	for_each(tr, tr + K, [](jthread& t) { t.join(); });

	return std::copy(first2, last2, d_first);
}

void binSearch() {

}

vector<dataT>::iterator mergeSmart(
	vector<dataT>::const_iterator first1, vector<dataT>::const_iterator last1,
	vector<dataT>::const_iterator first2, vector<dataT>::const_iterator last2,
	vector<dataT>::iterator d_first) {

	jthread tr[K];

	size_t cnt = (last1 - first1) / K;
	for (size_t i = 1; i <= cnt * K; i++) {
		upper_bound(first2, last2, *(first1 + K - 1));
		auto t = upper_bound(first2, last2, *(first1 + K - 1));

		tr[i - 1] = jthread(merge(first1, first1 + K - 1, first2, t, d_first));

		d_first += K + (t - first2);
		first1 += K;
	}

	for_each(tr, tr + K, [](jthread& t) { t.join(); });

	return std::copy(first2, last2, d_first);
}
*/


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