#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable : 4996)

using namespace std;

HWND hwnd;
HDC hdc;

enum color {
	BLUE, RED, WHITE, GREEN
};

void Draw(float val, int x_origin, int y_origin, int curx, color c) {
	if (c == BLUE) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(0, 0, 255));
	}
	else if (c == RED) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(255, 0, 0));
	}
	else if (c == GREEN) {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(0, 255, 0));
	}
	else {
		SetPixel(hdc, x_origin + curx, y_origin - val, RGB(255, 255, 255));
	}
}

void DrawHistogram(int histogram[400], int x_origin, int y_origin, int cnt) {
	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin + cnt, y_origin);

	MoveToEx(hdc, x_origin, 100, 0);
	LineTo(hdc, x_origin, y_origin);

	for (int CurX = 0; CurX < cnt; CurX++) {
		for (int CurY = 0; CurY < histogram[CurX]; CurY++) {
			MoveToEx(hdc, x_origin + CurX, y_origin, 0);
			LineTo(hdc, x_origin + CurX, y_origin - histogram[CurX] / 2);
		}
	}
}

void normalization(int histogram[400], float norm_histogram[400], int range, int rcnt) {
	for (int i = 0; i < range; i++) {
		norm_histogram[i] = (float)histogram[i] / 100;
	}
}

void DrawHistogram(float histogram[400], int x_origin, int y_origin, int cnt) {
	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin + cnt, y_origin);

	MoveToEx(hdc, x_origin, y_origin, 0);
	LineTo(hdc, x_origin, y_origin - 100);

	for (int CurX = 0; CurX < cnt; CurX++) {
		for (int CurY = 0; CurY < histogram[CurX]; CurY++) {
			MoveToEx(hdc, x_origin + CurX, y_origin, 0);
			LineTo(hdc, x_origin + CurX, y_origin - histogram[CurX]);
		}
	}
}

int Getfactorial(int n) {
	if (n == 0) {
		return 1;
	}

	int fac = n;
	for (int i = n - 1; i >= 1; i--) {
		fac *= i;
	}

	return fac;
}

float getERF(float x) {
	
	double pi = M_PI;
	double sqrt_pi = sqrt(pi);
	double mac_result = 0.0;
	
	for (int n = 0; n < 34; n++) {	// 35 �̻���ʹ� ǥ�ð� �ȵ� 
		mac_result = mac_result + (pow(-1., n) * pow(x, 2. * n + 1.) / Getfactorial(n) / (2. * n + 1.));
	}

	return 2.0 * mac_result / sqrt_pi;
	
	return erf(x);
}

float normal_cdf(float x, float rate, float mu, float sigma) {					// ��� ����� ����� ������ ���ڽ��ϴ�
	float input_erf = (x / rate - mu) / sqrt(sigma) / sqrt(2.0);
	input_erf = erf(input_erf);

	return (1.0 + input_erf) / 2.0;
}
/*
float inverse_normal_cdf(float p, float rate, float mu, float sigma, float tolerance = 0.00001)
{
	if (mu != 0 || sigma != 1)
		return mu + sigma * inverse_normal_cdf(p, rate, 0, 1, tolerance);		// normalization ����

	float low_z = -200.0, low_p = 0.0;					// ǥ�� ������ -200 ~ 200, Ȯ���� 0 ~ 1�̴�.
	float hi_z = 200.0, hi_p = 1.0;
	float mid_z, mid_p;									// ���� Ž���� ���� ã�� ���� �߰����� ���� ������ �����Ѵ�.

	
	while (hi_z - low_z > tolerance) {					// ǥ�� ������ ��� �������� Ŭ �� ���� �����Ѵ�.
		mid_z = (low_z + hi_z) / 2.0;
		mid_p = normal_cdf(mid_z, rate, mu, sigma);
		if (mid_p < p) {
			low_z = mid_z;
			low_p = mid_p;
		}
		else if (mid_p > p) {
			hi_z = mid_z;
			hi_p = mid_p;
		}
		else break;
	}

	return mid_z;
}*/

float inverse_normal_cdf(float p, float rate, float mu, float sigma, float tolerance = 0.00001)
{

	if (mu != 0 || sigma != 1)
		return mu + sigma * inverse_normal_cdf(p, rate, 0, 1, tolerance);		// normalization ����

	float zmiddle, pmiddle;									// ���� Ž���� ���� ã�� ���� �߰����� ���� ������ �����Ѵ�.
	float low_z = -200.0, low_p = 0.0;					// ǥ�� ������ -200 ~ 200, Ȯ���� 0 ~ 1�̴�.
	float hi_z = 200.0, hi_p = 1.0;
	float zleft = -200.0, zright = 200.0;
	float pleft = 0.0, pright = 1.0;

	while (tolerance < zright - zleft) {
		zmiddle = (zleft + zright) / 2.0;
		pmiddle = normal_cdf(zmiddle, rate, mu, sigma);
		
		if (pmiddle - p < 0.0) {
			zleft = zmiddle;
			pleft = pmiddle;
		}
		else if (pmiddle - p > 0.0) {
			zright = zmiddle;
			pright = pmiddle;
		}
		else break;
	}
	return zmiddle;
}

double gaussDistributeRand(int* value, int range, int rcnt, float average, float sigma) {		// Gauss ���� Random variable generation �Լ�

	int random = rand() % range;				// 0 ~ 400 ������ ���� ����
	int sum = 0;								// Uniform CDF ����� ���� ����

	for (int j = 0; j <= random; j++) {
		sum += value[j];						// Uniform CDF ���
	}
	
	float rand = (float)sum / (float)rcnt;	
	
	return 199.0 + inverse_normal_cdf(rand, range / 20.0, average * 20.0, sigma, 0.00001);		// -200 ~ 200 ���� -> 0 ~ 400 ���� ���ڷ� ����

}

float normal_pdf(int x, float rate, float  mu, float sigma) {
	double pi = M_PI;

	double sqrt_two_pi = sqrt(2 * pi);
	double _exp = exp(-pow(((double)x / rate - mu), 2) / (2 * pow(sigma, 2)));
	return _exp / (sqrt_two_pi * sigma);
}

int main(void)
{
	
	system("color F0");
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(hwnd);

	int y_axis = 300;

	int range = 400; // 0~400 ���� ���� ����
	int rcnt = 10000; // rcnt times ���� ���� �߻�
	float avg; // ���
	float sigma; // ǥ������

	cout << "Input avg: ";							// ��� �л� �Է� ������Ʈ
	cin >> avg;
	cout << "Input sigma: ";
	cin >> sigma;

	srand(time(NULL));
	int random_variable[401] = { 0, }; // ���� ���� count
	int Gaussian_random_variable[401] = { 0, }; // ����þ� ���� ī��Ʈ
	float norm_Gaussian_random_variable[401] = { 0, };
	float gaussianCDF[401] = { 0, }; // ���� ����þ� ���� CDF
	float gaussianCDF_true[401] = { 0, }; // ���� ���� ����þ� ���� CDF

	for (int i = 0; i < rcnt; i++) {				// LOOP for Uniform distribution random variable count
		int random = rand() % range;
		random_variable[random]++;
	}
	
	for (int i = 0; i < rcnt; i++) {				// LOOP for Gauss distribution  random variable count
		int random = gaussDistributeRand(random_variable, range, rcnt, avg, sigma);
		
		Gaussian_random_variable[random]++;
	}
	normalization(Gaussian_random_variable, norm_Gaussian_random_variable, range, rcnt);  // ����ȭ
	DrawHistogram(random_variable, 30, y_axis - 100, range);  // ���� ���� ���� ���
	DrawHistogram(Gaussian_random_variable, 500, y_axis, range); // ����þ� ���� ���� ���

	for (int i = 1; i < range; i++) {				// ��ȯ�� Gaussian random variable�� CDF�� ��� �� ����
		float cdf = 0.0;
		for (int j = 0; j <= i; j++) {
			cdf += norm_Gaussian_random_variable[j];
		}
		gaussianCDF[i] = cdf;
	}

	for (int i = 1; i < range; i++) {				// Gaussian�� ���� PDF��, CDF�� ���Ѵ�
		float value = normal_pdf(i - 1 * range / 2, range / 20, avg, sigma);
		gaussianCDF_true[i] = normal_cdf(i - 1 * range / 2, range / 20, avg, sigma) * 100.0;
		Draw(value * 250, 500, y_axis, i, RED);
	}

	DrawHistogram(gaussianCDF, 30, y_axis + 100, range); // true ����þ� cdf ���� ���
	//DrawHistogram(gaussianCDF_true, 30, y_axis + 300, range); // true ����þ� cdf ���� ���

	return 0;
}