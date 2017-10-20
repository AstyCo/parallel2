#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>

#include <math.h>
#include <string.h>


const char *byte_to_binary_n(int x, int id)
{
    static char b[10][26];
    if (id > 9)
        id = 0; // max - 9

    b[id][0] = 0;

    int z;
    for (z = 1 << 24; z > 0; z >>= 1)
    {
        strcat(b[id], ((x & z) == z) ? "1" : "0");
    }

    return b[id];
}

const char *byte_to_binary(int x)
{
    return byte_to_binary_n(x, 0);
}

template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

using namespace std;

struct Comparator
{
    int a;
    int b;
    int tact;

    Comparator() {}
    Comparator(int a_, int b_, int tact_)
        : a(a_), b(b_), tact(tact_) {}

    bool operator<(const Comparator &another) const
    {
        return this->tact < another.tact;
    }
};



int findMaxTact(const vector<Comparator> &v)
{
    if (v.size() == 0)
        return -1;
    unsigned maxTactIndex = 0;
    for (unsigned i = 1; i < v.size(); ++i)
        if (v[i].tact > v[maxTactIndex].tact)
            maxTactIndex = i;

    return v[maxTactIndex].tact;
}

string toReport(const vector<Comparator> &result)
{
    ostringstream stream;
    for (unsigned i = 0; i < result.size(); ++i)
        stream << toString(result.at(i).a) << ' '
               << toString(result.at(i).b) << ' '
               << toString(result.at(i).tact) << endl;
    stream << "n_tact " << toString(findMaxTact(result)) << endl
           << "n_comp " << toString(result.size()) << endl;
    return stream.str();
}

vector<Comparator> result;
void reverseResultTacts()
{
    int maxTactP1 = findMaxTact(result) + 1;
    for (unsigned i = 0; i < result.size(); ++i)
        result[i].tact = maxTactP1 - result[i].tact;
}

void add_comparator(int a, int b, int tact)
{
    result.push_back(Comparator(a, b, tact));
}

void S(int a, int b, int d, int n, int m, int tact)
{
    if (n == 0 || m == 0)
        return;
    if (n == 1 && m == 1) {
        add_comparator(a, b, tact + 1);
        return;
    }
    int halfN = n - n / 2;
    int halfM = m - m / 2;
    int i;

    S(a, b, 2 * d, halfN, halfM, tact + 1);
    S(a + d, b + d, 2 * d, n - halfN, m - halfM, tact + 1);

    for (i = 1; i < n - 1; i += 2)
        add_comparator(a + d * i, a + d * (i + 1), tact + 1);

    if (n % 2 == 0) {
        add_comparator(a + d * (n - 1), b, tact + 1);
        i = 1;
    }
    else {
        i = 0;
    }

    for (; i < m - 1; i += 2)
        add_comparator(b + d * i, b + d * (i + 1), tact + 1);
}

void B0(int first, int step, int count, int tact)
{
    if (count < 2)
        return;
    if (count == 2) {
        add_comparator(first, first + step, tact + 1);
        return;
    }
    int half = count - count / 2;
    int countOfMergeSteps = ceil(log2(half)) + 1;
    B0(first, step, half, tact + countOfMergeSteps);
    B0(first + step * half, step, count - half, tact + countOfMergeSteps);
    S(first, first + step * half, step, half, count - half, tact);
}

void B(int first, int step, int count)
{
    result.clear();
    B0(first, step, count, 1);
    reverseResultTacts();
}

int makeAnswer(int permutation)
{
    int oneCount = 0;
    for (int i = 1, r = 1; i < 25; ++i, r <<= 1) {
        if (r & permutation)
            ++oneCount;
    }
    return (1 << oneCount) - 1;
}

void swap_1_0(int a, int b, int &array, int array_size)
{
    const int a_mask = 1 << (array_size - a);
    const int b_mask = 1 << (array_size - b);

    if ((array & a_mask) && !(array & b_mask)) {
        array ^= a_mask | b_mask;
    }
}

int sorted(int array, int size)
{
    for (unsigned i = 0; i < result.size(); ++i)
        swap_1_0(result[i].a, result[i].b, array, size);
    return array;
}

void my_assert(int permutation, int answer, int size)
{
    int result = sorted(permutation, size);
    if (result != answer) {
        cout << "ERROR: " << byte_to_binary(permutation) << ' '
             << byte_to_binary_n(answer, 1) << ' '
             << byte_to_binary_n(result, 2) << ' ' << size << endl;
        exit(1);
    }
}

bool test(int n)
{
    B(1, 1, n);
    sort(result.begin(), result.end());
    int lastPermutation = 1 << n;
    for (int permutation = 0; permutation < lastPermutation; ++permutation) {
        int answer = makeAnswer(permutation);
        my_assert(permutation, answer, n);
//        cout << "done " << toString(permutation) << endl;
    }
    return true;
}

void makeMyReportToFile(int n)
{
    ofstream myfile;
    myfile.open ("debug.txt", std::ofstream::out);
    B(1, 1, n);

    cout << toReport(result) << endl;
    sort(result.begin(), result.end());
    myfile << toReport(result);
    myfile.close();
}

int main(int /*argc*/, char */*argv*/[])
{
    makeMyReportToFile(16);
//    return 0;
#define EXERCISE 2
    clock_t begin = clock();
#   if EXERCISE == 2
    for (int i = 1; i <= 24; i += 1) {
        if (test(i))
            cout << "success " << toString(i) << endl;
    }
#   endif
#   if EXERCISE == 3
    B(1, 1, 10000);
    cout << toReport(result) << endl;
#   endif
    clock_t end = clock();
    double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed " << toString((int) elapsedSecs) << "secs." << endl;
    return 0;
}
