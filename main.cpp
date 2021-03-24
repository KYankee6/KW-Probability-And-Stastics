#include <iostream>
#include <random>
#include <vector>
#include "pbplots.hpp"
#include "supportLib.hpp"

#define LOOP 100.0

using namespace std;

int main()
{
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> dis(0.0, 1.0);

    vector<double> xs;
    vector<double> ys;
    for (int n = 0; n <= LOOP; ++n)
    {
        xs.push_back((double)n / LOOP);
        ys.push_back(dis(gen));
    }
    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = &xs;
    series->ys = &ys;
    series->linearInterpolation = false;
    series->pointType = toVector(L"dots");
    series->color = CreateRGBColor(1, 0, 0);
    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->xMin = -0.2;
    settings->xMax = 1.2;
    settings->yMin = -0.4;
    settings->yMax = 2;
    settings->width = 600;
    settings->height = 400;
    settings->autoBoundaries = false;
    settings->autoPadding = false;
    settings->title = toVector(L"");
    settings->xLabel = toVector(L"");
    settings->yLabel = toVector(L"");
    settings->scatterPlotSeries->push_back(series);
    DrawScatterPlotFromSettings(imageReference, settings);

    vector<double> *pngdata = ConvertToPNG(imageReference->image);
    WriteToFile(pngdata, "example1.png");
    DeleteImage(imageReference->image);
}