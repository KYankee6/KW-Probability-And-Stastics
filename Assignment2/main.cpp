#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
using namespace std;

#define INPUT_PATH "./gray/gAirplane350_555.raw"
#define TARGET_PATH "./gray/barbara(512x512).raw"
#define OUTPUT_PATH "./gray/result.raw"
#define INPUT_WIDTH 350
#define INPUT_HEIGHT 555
#define TARGET_WIDTH 512
#define TARGET_HEIGHT 512
#define BUCKET_SIZE 256
#define DIF_MAX 256 * INPUT_WIDTH *INPUT_HEIGHT

FILE *input_stream = fopen(INPUT_PATH, "rb");
FILE *target_stream = fopen(TARGET_PATH, "rb");
FILE *output_stream = fopen(OUTPUT_PATH, "wt");
unsigned char input_arr[INPUT_WIDTH][INPUT_HEIGHT], target_arr[TARGET_WIDTH][TARGET_HEIGHT], output_arr[INPUT_WIDTH][INPUT_HEIGHT];
double input_hist[BUCKET_SIZE] = {}, target_hist[BUCKET_SIZE] = {}, output_hist[BUCKET_SIZE] = {};
double input_chist[BUCKET_SIZE] = {}, target_chist[BUCKET_SIZE] = {}, output_chist[BUCKET_SIZE] = {};
void printResult();
void getPDF(int selector, int WIDTH, int HEIGHT);
void getCDF(bool trig);
int InverseTargetCDF(double s);
void histogramMatching();
int main()
{
    bool isOutputArrayReady = false;
    if (!input_stream || !target_stream)
    {
        printf("File Not Exist\n");
        return 0;
    }
    fread(input_arr, sizeof(char), INPUT_WIDTH * INPUT_HEIGHT, input_stream);     //GET INPUT IMAGE INTO ARRAY
    fread(target_arr, sizeof(char), TARGET_WIDTH * TARGET_HEIGHT, target_stream); //GET TARGET IMAGE INTO ARRAY
    getPDF(0, INPUT_WIDTH, INPUT_HEIGHT);                                         //get PDF of input
    getPDF(1, TARGET_WIDTH, TARGET_WIDTH);                                        //get PDF of target
    getCDF(isOutputArrayReady);                                                   //get input, target image's CDF
    histogramMatching();                                                          //call histo match
    getPDF(2, INPUT_WIDTH, INPUT_HEIGHT);                                         //get PDF OF OUTPUT
    getCDF(isOutputArrayReady = true);                                            //get CDF of OUTPUT
    printResult();                                                                //for histogram visualization
    fwrite(output_arr, sizeof(char), INPUT_WIDTH * INPUT_HEIGHT, output_stream);  //write output image
    fclose(input_stream);
    fclose(target_stream);
    fclose(output_stream);

    return 1;
}
void getPDF(int selector, int WIDTH, int HEIGHT)
{
    if (selector == 0)
    {
        //input array HISTOGRAM
        for (int i = 0; i < WIDTH; i++)
        {
            for (int j = 0; j < HEIGHT; j++)
            {
                int intensity = input_arr[i][j]; //r_k, as for gray levels of the input images
                input_hist[intensity]++;         //n_k, as for numbers of pixel of that intensity
                //input_hist[intensity] /= BUCKET_SIZE;
            }
        }
        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            input_hist[i] /= WIDTH * HEIGHT;
        }
    }

    else if (selector == 1)
    {
        //target array HISTOGRAM
        for (int i = 0; i < WIDTH; i++)
        {
            for (int j = 0; j < HEIGHT; j++)
            {
                int intensity = target_arr[i][j];
                target_hist[intensity]++;
                //target_hist[intensity] /= BUCKET_SIZE;
            }
        }
        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            target_hist[i] /= WIDTH * HEIGHT;
        }
    }
    else
    {
        //output array PDF
        for (int i = 0; i < WIDTH; i++)
        {
            for (int j = 0; j < HEIGHT; j++)
            {
                int intensity = output_arr[i][j];
                output_hist[intensity]++;
            }
        }
        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            output_hist[i] /= WIDTH * HEIGHT;
        }
    }
}

void getCDF(bool trig)
{
    if (!trig)
    {
        input_chist[0] = input_hist[0];
        target_chist[0] = target_hist[0];

        //get input array's density function
        //get target array's density function

        //s=T(r)=sum(from 0 to r)p_r(w)dw
        for (int i = 1; i < BUCKET_SIZE; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                input_chist[i] += input_hist[j];
                target_chist[i] += target_hist[j];
            }
        }
        // for (int i = 0; i < BUCKET_SIZE; i++)
        // {
        //     input_chist[i] /= INPUT_WIDTH * INPUT_HEIGHT;    //Normalization
        //     target_chist[i] /= TARGET_WIDTH * TARGET_HEIGHT; //Normalization
        // }
    }
    else
    {
        output_chist[0] = output_hist[0];
        for (int i = 0; i < BUCKET_SIZE - 1; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                output_chist[i] += output_hist[j];
            }
        }
        // for (int i = 0; i < BUCKET_SIZE; i++)
        // {
        //     output_chist[i] /= INPUT_WIDTH * INPUT_HEIGHT; //Normalization
        // }
    }
}
int InverseTargetCDF(double s)
{
    double minDiff = INT_MAX;
    int z = 0;
    for (int i = 0; i < BUCKET_SIZE; i++)
    {
        if (minDiff > abs(target_chist[i] - s))
        {
            minDiff = abs(target_chist[i] - s); //find inverse of CDF_b
            z = i;
        }
    }
    return z;
}
void histogramMatching()
{
    for (int i = 0; i < INPUT_WIDTH; i++)
    {
        for (int j = 0; j < INPUT_HEIGHT; j++)
        {
            double CDF_A = input_chist[input_arr[i][j]];
            int z = InverseTargetCDF(CDF_A); //find CDF_b^-1(CDF_a)
            output_arr[i][j] = z;            //monotonic increase
        }
    }
}
void printResult()
{
    //pretty self-explanatory
    cout << "idx,"
         << "input hist,"
         << "target hist,"
         << "output hist,"
         << "input chist,"
         << "target chist,"
         << "output chist,"
         << endl;

    for (int i = 0; i < BUCKET_SIZE; i++)
    {
        cout << i << "," << input_hist[i] << "," << target_hist[i] << "," << output_hist[i] << "," << input_chist[i] << "," << target_chist[i] << "," << output_chist[i] << endl;
    }
}