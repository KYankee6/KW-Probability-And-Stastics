#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <cmath>
#define LOOP 10000.0
using namespace std;
ofstream UD_file;
ofstream LPD_file;
ofstream KS_LPD_file;

template <typename T>
int sgn(T val) //signum function
{
    return (T(0) < val) - (val < T(0));
}
void UDRVtoLPDRV(vector<double> UD_v, vector<double> LPD_v, int mu, int b);
double Transformation(double rv, int mu, int b);
double laplaceCDF(double rv, int mu, int b);
double laplacePDF(double x, int mu, int b);
double KSTEST_CRIT();
double Dvalue(vector<double> H0, vector<double> H1);

int main()
{
    default_random_engine gen;
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_real_distribution<> KSdis(-15.0, 15.0);

    double rv;
    double KSrv;
    cout << "input mu, b : " << endl;
    int mu, b;
    cin >> mu >> b;
    UD_file.open("UDwords10000.txt"); //files for saves the distributions
    LPD_file.open("LPDwords10000.txt");
    KS_LPD_file.open("KSLPDwords10000.txt");

    vector<double> UD_v;
    vector<double> LPD_v;

    for (int n = 0; n <= LOOP; ++n)
    {
        rv = dis(gen);     //generate random uniform distribution
        KSrv = KSdis(gen); //generate random laplace distribution

        UD_v.push_back(rv);
        string srv = to_string(rv);
        KSrv = laplaceCDF(KSrv, mu, b); //get laplace distribution (PDF,CDF)
        string KSsrv = to_string(rv);
        UD_file << srv << endl; //store to the in to vectors and filestream
        KS_LPD_file << KSsrv << endl;
    }

    cout << "mu: " << mu << "b:" << b << endl;
    UDRVtoLPDRV(UD_v, LPD_v, mu, b); //transformation Uniformdistribution to Laplace distribution
    cout << KSTEST_CRIT();           //get K-S test's critical value of D(alpha =0.05, 95%)
    UD_file.close();
    LPD_file.close();
    KS_LPD_file.close();
}
void UDRVtoLPDRV(vector<double> UD_v, vector<double> LPD_v, int mu, int b)
{
    // Y = T(X) , Y and X are Random Variables, and function T is transformation function
    // assume that T(x) is monotonically increasing
    // since we Know f_x(x) as Uniform Distribution,
    // as for F_y[y=T(x)] = F_x(x),
    // Since Uniform Distribution x, F_x(x)=x
    // y=T(x)=inverse(F_y(F_x(x)))
    // Therefore, y=T(x)=inverse(F_y(x))

    //Let's define function T(x)
    //y=T(x)=inv(F_y(x))

    //T(x)=mu-b*sgn(x-0.5)ln(1-2|x-0.5|)
    double res;
    for (int i = 0; i < UD_v.size(); i++)
    {
        res = Transformation(UD_v.at(i), mu, b);
        LPD_v.push_back(res);
        LPD_file << res << endl;
    }
}
double Transformation(double rv, int mu, int b)
{
    return mu - (b * sgn(rv - 0.5)) * log(1 - 2 * (abs(rv - 0.5))); //https://en.wikipedia.org/wiki/Laplace_distribution
}
double laplaceCDF(double rv, int mu, int b)
{
    //https://en.wikipedia.org/wiki/Laplace_distribution
    if (rv >= mu)
        return (1.0 / 2.0) * (exp(-(abs(rv - mu)) / b));
    else
        return 1.0 - (1.0 / 2.0) * (exp(-(abs(rv - mu)) / b));
}
double laplacePDF(double x, int mu, int b)
{
    //https://en.wikipedia.org/wiki/Laplace_distribution
    double res = (1.0 / (2.0 * b)) * exp((((-1.0) * abs(x - mu)) / b));
    return res;
}
double Dvalue(vector<double> H0, vector<double> H1)
{
    double res = 0;
    for (int i = 0; i <= LOOP; i++)
    {
        if (res > abs(H0.at(i) - H1.at(i))) //get Dvalue, which is maximum difference of two distribution's CDF
            res = abs(H0.at(i) - H1.at(i));
    }
    return res;
}
double KSTEST_CRIT()
{
    return 1.36 / sqrt(LOOP); //alpha = 0.05, <95%
}