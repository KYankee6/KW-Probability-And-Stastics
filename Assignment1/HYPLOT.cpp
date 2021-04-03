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
	
	for (int n = 0; n < 34; n++) {	// 35 이상부터는 표시가 안됨 
		mac_result = mac_result + (pow(-1., n) * pow(x, 2. * n + 1.) / Getfactorial(n) / (2. * n + 1.));
	}

	return 2.0 * mac_result / sqrt_pi;
	
	return erf(x);
}

float normal_cdf(float x, float rate, float mu, float sigma) {					// 계산 방법과 결과는 보고서에 쓰겠습니다
	float input_erf = (x / rate - mu) / sqrt(sigma) / sqrt(2.0);
	input_erf = erf(input_erf);

	return (1.0 + input_erf) / 2.0;
}
/*
float inverse_normal_cdf(float p, float rate, float mu, float sigma, float tolerance = 0.00001)
{
	if (mu != 0 || sigma != 1)
		return mu + sigma * inverse_normal_cdf(p, rate, 0, 1, tolerance);		// normalization 실행

	float low_z = -200.0, low_p = 0.0;					// 표현 범위는 -200 ~ 200, 확률은 0 ~ 1이다.
	float hi_z = 200.0, hi_p = 1.0;
	float mid_z, mid_p;									// 이진 탐색을 통해 찾기 위해 중간값을 받을 변수를 선언한다.

	
	while (hi_z - low_z > tolerance) {					// 표현 범위가 허용 오차보다 클 때 까지 실행한다.
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
		return mu + sigma * inverse_normal_cdf(p, rate, 0, 1, tolerance);		// normalization 실행

	float zmiddle, pmiddle;									// 이진 탐색을 통해 찾기 위해 중간값을 받을 변수를 선언한다.
	float low_z = -200.0, low_p = 0.0;					// 표현 범위는 -200 ~ 200, 확률은 0 ~ 1이다.
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

double gaussDistributeRand(int* value, int range, int rcnt, float average, float sigma) {		// Gauss 분포 Random variable generation 함수

	int random = rand() % range;				// 0 ~ 400 사이의 랜덤 숫자
	int sum = 0;								// Uniform CDF 계산을 위한 숫자

	for (int j = 0; j <= random; j++) {
		sum += value[j];						// Uniform CDF 계산
	}
	
	float rand = (float)sum / (float)rcnt;	
	
	return 199.0 + inverse_normal_cdf(rand, range / 20.0, average * 20.0, sigma, 0.00001);		// -200 ~ 200 사이 -> 0 ~ 400 사이 숫자로 변경

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

	int range = 400; // 0~400 범위 랜덤 변수
	int rcnt = 10000; // rcnt times 랜덤 변수 발생
	float avg; // 평균
	float sigma; // 표준편차

	cout << "Input avg: ";							// 평균 분산 입력 프롬프트
	cin >> avg;
	cout << "Input sigma: ";
	cin >> sigma;

	srand(time(NULL));
	int random_variable[401] = { 0, }; // 랜덤 변수 count
	int Gaussian_random_variable[401] = { 0, }; // 가우시안 변수 카운트
	float norm_Gaussian_random_variable[401] = { 0, };
	float gaussianCDF[401] = { 0, }; // 랜덤 가우시안 분포 CDF
	float gaussianCDF_true[401] = { 0, }; // 실제 정답 가우시안 분포 CDF

	for (int i = 0; i < rcnt; i++) {				// LOOP for Uniform distribution random variable count
		int random = rand() % range;
		random_variable[random]++;
	}
	
	for (int i = 0; i < rcnt; i++) {				// LOOP for Gauss distribution  random variable count
		int random = gaussDistributeRand(random_variable, range, rcnt, avg, sigma);
		
		Gaussian_random_variable[random]++;
	}
	normalization(Gaussian_random_variable, norm_Gaussian_random_variable, range, rcnt);  // 정규화
	DrawHistogram(random_variable, 30, y_axis - 100, range);  // 랜덤 변수 분포 출력
	DrawHistogram(Gaussian_random_variable, 500, y_axis, range); // 가우시안 변수 분포 출력

	for (int i = 1; i < range; i++) {				// 변환한 Gaussian random variable의 CDF를 계산 후 저장
		float cdf = 0.0;
		for (int j = 0; j <= i; j++) {
			cdf += norm_Gaussian_random_variable[j];
		}
		gaussianCDF[i] = cdf;
	}

	for (int i = 1; i < range; i++) {				// Gaussian의 실제 PDF와, CDF를 구한다
		float value = normal_pdf(i - 1 * range / 2, range / 20, avg, sigma);
		gaussianCDF_true[i] = normal_cdf(i - 1 * range / 2, range / 20, avg, sigma) * 100.0;
		Draw(value * 250, 500, y_axis, i, RED);
	}

	DrawHistogram(gaussianCDF, 30, y_axis + 100, range); // true 가우시안 cdf 정답 출력
	//DrawHistogram(gaussianCDF_true, 30, y_axis + 300, range); // true 가우시안 cdf 정답 출력

	return 0;
}