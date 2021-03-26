#include "matplotlibcpp.h"
#include <random>
#include <vector>
#include <
#define LOOP 1.0
using namespace std;
namespace plt = matplotlibcpp;
int main()
{
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> dis(0.0, 1.0);

    vector<double> xs;
    vector<double> ys;
    for (int n = 0; n <= LOOP; ++n)
    {
        xs.push_back((double)n / LOOP);
        ys.push_back(dis(gen));
        //  fprintf(fp, sRV);
    }
    plt::bar(xs, ys, "black", "-", 0.01, {{"align", "edge"}}, {{"width", "0.01"}});
    plt::save("./Uniform_Distribution.png");
}
