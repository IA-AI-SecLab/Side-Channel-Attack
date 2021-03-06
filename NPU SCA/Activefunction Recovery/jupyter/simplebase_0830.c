#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // memset함수 
#include <math.h>  // expf함수 
#include "simpleserial.h"

#define INPUT 3		// 입력층 노드 개수 
#define HIDDEN 6	// 첫번째 은닉층 노드 개수 
#define OUTPUT 2	// 출력층 노드 개수 

// 입,출력 데이터 

// 입력 데이터 
float X[INPUT] = { 0.0 };  // ※ Setting ※

float Y[OUTPUT];	

// Weight ( ※ Setting ※ )
float WX[INPUT][HIDDEN]; // 입력층 - 은닉층(1) 가중치(Weight)  // 왜 INPUT + 1인가?
float WH[HIDDEN][OUTPUT]; // 은닉층(1) - 출력층 가중치(Weight)	

// Weight Sum
float WSX[HIDDEN]; // 입력값으로 계산한 은닉층(1)에 들어갈 가중합
float WSH[OUTPUT]; // 은닉층(1)의 출력값으로 계산한 출력층에 들어갈 가중합 

// Activefunction Result
float AFR[HIDDEN];


// Active Function Implement
// expf : float 입출력 exp 함수 -> 존재하지 않음  

// Sigmoid 
float sigmoid(float h)
{
	return (1 / (1 + exp((-1)*h)));
}

// tanhf : float 입출력 Tanh함수 
float tanh2(float h)
{
   return (2 / (1 + exp(-2 * h)) - 1);

}

// ReLU 
float relu(float h)
{
	return (h > 0 ? h : 0);
}

float Hardsigmoid(float h)
{
	return (h > 2.5 ? 1 : (h < -2.5 ? 0 : 0.2*h + 0.5));
}

float LeaklyReLU(float h)
{
	return (h > 0 ? h : 0.01*h);
}


float ELU(float h)
{
	return (h > 0 ? h : exp(h) - 1);
}


float Softplus(float h)
{
	return log(exp(h) + 1);
}


float Softsign(float h)
{
	return (h / (1 + fabs(h)));
}

float Swish(float h)
{
	return (1 / (1 + exp(-h))) * h;
}



// 선택 입력
int count = 0;
float in = -3; // 10 

// 500개 
float input[2000] = {
4.359 , 1.260 , 2.870 ,3.800 , -0.320 , 0.920 ,2.735 , -1.260 , -0.750 ,
1.171 , 0.060 , -0.530 ,8.831 , -1.980 , 2.790 ,2.227 , -1.760 , -1.590 ,
-0.251 , 2.780 , 1.180 ,2.949 , 2.050 , 2.690 ,3.067 , -0.430 , 0.320 ,
0.314 , 2.010 , 0.820 ,3.849 , 1.590 , 2.870 ,3.664 , 0.950 , 2.110 ,
-0.611 , -0.500 , -2.300 ,3.311 , -1.010 , -0.070 ,3.306 , -1.070 , -0.130 ,
5.362 , -0.570 , 1.800 ,4.897 , -2.620 , -0.550 ,1.563 , -0.680 , -0.950 ,
-4.692 , 2.800 , -1.860 ,-2.503 , 1.630 , -1.490 ,4.400 , -1.200 , 0.520 ,
2.609 , -2.600 , -2.110 ,3.347 , 0.010 , 0.990 ,-2.851 , 2.470 , -0.890 ,
-0.104 , 1.170 , -0.260 ,9.995 , -2.940 , 2.710 ,-2.642 , 0.940 , -2.250 ,
6.686 , -2.940 , 0.410 ,1.104 , -0.560 , -1.120 ,6.081 , -2.510 , 0.420 ,
-0.947 , -0.870 , -2.850 ,-3.792 , 1.200 , -2.780 ,3.542 , -2.490 , -1.320 ,
2.212 , 2.040 , 2.240 ,2.512 , -1.750 , -1.300 ,3.324 , 0.060 , 1.060 ,
0.080 , 0.410 , -0.850 ,3.454 , -0.250 , 0.850 ,1.513 , -2.160 , -2.390 ,
4.032 , -2.390 , -0.860 ,4.466 , -0.930 , 0.890 ,3.567 , 0.680 , 1.860 ,
0.122 , 1.960 , 0.730 ,6.365 , -0.800 , 2.350 ,-4.996 , 2.030 , -2.760 ,
0.585 , 0.110 , -0.770 ,-1.695 , 0.550 , -1.920 ,-1.268 , 0.610 , -1.560 ,
2.865 , -2.170 , -1.430 ,1.376 , 0.330 , 0.010 ,5.906 , -1.780 , 1.080 ,
0.116 , 0.150 , -1.040 ,2.842 , 0.310 , 1.020 ,2.502 , 0.890 , 1.360 ,
1.284 , 2.290 , 1.900 ,-0.112 , 0.350 , -0.990 ,6.278 , -2.170 , 0.970 ,
1.318 , 2.540 , 2.180 ,8.824 , -2.230 , 2.690 ,7.301 , -1.060 , 2.790 ,
5.904 , -2.100 , 0.790 ,7.836 , -2.210 , 2.030 ,-1.816 , 1.650 , -0.870 ,
7.110 , -2.190 , 1.550 ,-5.594 , 2.980 , -2.180 ,1.718 , -2.280 , -2.290 ,
5.658 , -2.500 , 0.240 ,0.603 , 1.580 , 0.760 ,3.314 , 1.690 , 2.760 ,
3.106 , 1.460 , 2.390 ,-3.551 , 1.990 , -1.720 ,7.298 , -1.880 , 2.010 ,
2.280 , 0.810 , 1.180 ,-4.697 , 2.020 , -2.480 ,3.965 , -1.220 , 0.350 ,
-5.499 , 2.710 , -2.350 ,4.120 , -0.860 , 0.820 ,0.179 , 0.860 , -0.220 ,
-5.370 , 2.530 , -2.430 ,1.055 , -1.500 , -1.940 ,0.897 , 0.700 , 0.130 ,
0.156 , 1.390 , 0.300 ,8.674 , -1.900 , 2.980 ,2.977 , -2.450 , -1.530 ,
1.404 , -0.800 , -0.990 ,6.031 , -1.190 , 1.850 ,2.320 , 1.510 , 1.940 ,
0.157 , -1.780 , -2.820 ,-1.909 , 0.500 , -2.000 ,4.244 , -1.540 , 0.270 ,
-4.140 , 2.660 , -1.410 ,-1.921 , 2.500 , -0.020 ,-4.986 , 2.290 , -2.360 ,
-1.712 , 0.680 , -1.670 ,4.947 , -2.100 , 0.220 ,1.599 , 0.900 , 0.860 ,
6.317 , -1.150 , 2.120 ,0.797 , -0.420 , -1.000 ,2.934 , 1.690 , 2.580 ,
-2.691 , 0.130 , -2.880 ,2.718 , -1.730 , -0.950 ,7.686 , -1.370 , 2.870 ,
-4.724 , 2.360 , -2.080 ,-2.459 , 0.470 , -2.370 ,4.236 , 1.130 , 2.950 ,
0.799 , -1.010 , -1.560 ,2.164 , 0.380 , 0.770 ,0.760 , 2.780 , 2.170 ,
5.194 , 0.010 , 2.520 ,3.227 , -1.720 , -0.560 ,0.460 , -0.230 , -1.010 ,
-5.328 , 2.780 , -2.060 ,5.996 , -1.930 , 1.170 ,3.450 , -0.090 , 1.220 ,
-3.732 , 1.870 , -1.840 ,7.202 , -2.110 , 1.840 ,0.253 , -1.900 , -2.790 ,
3.248 , 1.000 , 2.170 ,-3.148 , 1.660 , -1.630 ,5.325 , -1.760 , 0.890 ,
-1.126 , 2.070 , 0.190 ,-1.978 , 1.050 , -1.410 ,5.177 , -2.150 , 0.410 ,
1.237 , 0.600 , 0.390 ,-5.124 , 1.730 , -2.920 ,2.394 , -1.200 , -0.580 ,
8.590 , -2.270 , 2.680 ,-0.461 , 1.420 , 0.030 ,7.406 , -2.220 , 1.910 ,
1.501 , 0.680 , 0.670 ,2.097 , -2.480 , -2.040 ,3.129 , -1.330 , -0.180 ,
3.882 , 0.370 , 2.030 ,1.940 , 1.360 , 1.660 ,8.866 , -2.820 , 2.350 ,
-4.030 , 1.040 , -2.810 ,3.480 , 1.460 , 2.840 ,5.805 , -1.200 , 1.830 ,
-0.043 , 2.590 , 1.510 ,1.075 , 0.840 , 0.560 ,-4.347 , 2.340 , -1.730 ,
-3.476 , 2.300 , -1.160 ,0.271 , -1.340 , -2.150 ,8.529 , -2.710 , 2.250 ,
1.533 , -2.800 , -2.710 ,-5.656 , 2.540 , -2.430 ,0.396 , 2.490 , 1.740 ,
8.458 , -2.570 , 2.350 ,6.168 , -0.830 , 2.480 ,0.532 , -1.170 , -1.780 ,
4.381 , 0.270 , 2.330 ,7.431 , -1.990 , 2.220 ,4.458 , -2.760 , -0.610 ,
2.601 , 2.080 , 2.890 ,2.558 , -2.600 , -1.770 ,-3.338 , 0.940 , -2.370 ,
3.040 , 1.510 , 2.640 ,3.583 , -1.370 , 0.170 ,2.078 , -1.920 , -1.420 ,
4.251 , -2.230 , -0.210 ,6.145 , -1.040 , 2.290 ,0.704 , -1.790 , -2.240 ,
-2.587 , 2.210 , -0.570 ,1.860 , -2.630 , -2.260 ,-0.511 , -1.590 , -2.880 ,
-0.488 , 1.240 , -0.060 ,1.923 , -2.430 , -2.010 ,-1.996 , 2.730 , 0.370 ,
4.469 , 0.450 , 2.620 ,6.483 , -1.920 , 1.680 ,2.585 , 1.730 , 2.580 ,
2.452 , -2.150 , -1.350 ,-3.233 , 1.000 , -2.190 ,-0.329 , 1.630 , 0.460 ,
5.640 , -2.240 , 0.790 ,4.458 , -2.280 , -0.070 ,-0.099 , -1.490 , -2.460 ,
0.656 , -1.590 , -2.030 ,1.758 , -0.590 , -0.270 ,-0.781 , -1.120 , -2.560 ,
-3.212 , 2.730 , -0.440 ,3.215 , -0.200 , 1.140 ,3.357 , -1.000 , 0.450 ,
8.526 , -2.620 , 2.450 ,2.807 , -0.770 , 0.300 ,-4.950 , 2.090 , -2.270 ,
1.852 , -0.750 , -0.340 ,1.334 , 2.390 , 2.410 ,-0.221 , 1.380 , 0.330 ,
-0.869 , 2.500 , 0.990 ,0.867 , -2.230 , -2.480 ,8.597 , -2.450 , 2.690 ,
-1.725 , 0.720 , -1.360 ,9.273 , -2.780 , 2.840 ,3.759 , -2.570 , -0.790 ,
-3.974 , 1.900 , -1.750 ,3.687 , -1.060 , 0.660 ,5.020 , -0.920 , 1.730 ,
2.524 , -0.550 , 0.360 ,5.603 , -1.740 , 1.330 ,-3.535 , 0.880 , -2.440 ,
1.836 , 0.360 , 0.790 ,-5.193 , 1.960 , -2.520 ,8.606 , -2.420 , 2.760 ,
-0.535 , 1.990 , 0.760 ,1.352 , -1.170 , -1.050 ,0.975 , -0.200 , -0.350 ,
-2.252 , 2.220 , -0.200 ,-5.103 , 1.910 , -2.490 ,5.599 , -1.210 , 1.880 ,
-2.149 , 1.250 , -1.080 ,-0.911 , -0.150 , -1.600 ,6.488 , -2.410 , 1.320 ,
-2.793 , 1.290 , -1.480 ,2.774 , 1.390 , 2.500 ,0.768 , -0.070 , -0.340 ,
-1.683 , 0.440 , -1.540 ,0.687 , -1.140 , -1.450 ,-1.672 , 1.790 , -0.190 ,
-1.678 , 0.630 , -1.340 ,6.394 , -1.700 , 1.980 ,0.010 , -1.130 , -1.900 ,
6.020 , -1.200 , 2.220 ,0.007 , -1.790 , -2.550 ,2.841 , 0.880 , 2.070 ,
-5.569 , 2.260 , -2.420 ,4.850 , -0.570 , 2.040 ,0.348 , -0.940 , -1.460 ,
-4.551 , 2.040 , -1.920 ,8.407 , -2.900 , 2.220 ,-2.370 , 0.580 , -1.840 ,
-0.839 , 2.490 , 1.120 ,3.021 , -2.270 , -0.900 ,-1.401 , -0.060 , -1.790 ,
4.342 , -2.710 , -0.410 ,0.745 , 0.980 , 0.740 ,-0.801 , 2.520 , 1.190 ,
0.818 , 0.630 , 0.450 ,3.077 , -2.670 , -1.240 ,1.530 , -2.170 , -1.820 ,
0.206 , 0.840 , 0.240 ,2.641 , -1.190 , -0.070 ,1.758 , 1.470 , 1.950 ,
-0.890 , -0.640 , -1.980 ,-0.774 , -0.260 , -1.520 ,-0.447 , -1.180 , -2.200 ,
3.420 , -0.570 , 1.100 ,-5.234 , 1.840 , -2.540 ,-5.866 , 2.050 , -2.770 ,
0.004 , 0.310 , -0.400 ,0.569 , 2.940 , 2.600 ,-3.963 , 0.550 , -2.920 ,
3.231 , -1.970 , -0.400 ,3.620 , -1.600 , 0.240 ,0.547 , -1.410 , -1.710 ,
2.386 , -0.020 , 0.950 ,-3.582 , 1.460 , -1.740 ,-0.162 , -0.990 , -1.780 ,
7.114 , -1.810 , 2.480 ,2.114 , -0.830 , -0.030 ,-2.682 , 2.280 , -0.290 ,
-0.923 , 2.120 , 0.780 ,4.787 , -1.750 , 0.930 ,6.294 , -1.520 , 2.210 ,
-1.767 , 2.120 , 0.200 ,-5.610 , 2.680 , -1.920 ,4.168 , 0.190 , 2.430 ,
0.966 , -2.510 , -2.470 ,2.017 , -2.060 , -1.290 ,1.946 , 1.250 , 1.940 ,
-1.573 , 1.390 , -0.370 ,5.379 , -2.960 , 0.170 ,6.330 , -2.450 , 1.340 ,
-4.565 , 2.770 , -1.080 ,0.879 , 1.340 , 1.300 ,-3.565 , 1.030 , -2.100 ,
-4.421 , 1.660 , -2.070 ,1.652 , -1.950 , -1.410 ,2.442 , 0.320 , 1.390 ,
-5.489 , 2.070 , -2.400 ,-3.412 , 1.120 , -1.890 ,1.083 , 0.500 , 0.630 ,
-1.453 , 1.110 , -0.530 ,0.156 , 1.490 , 0.970 ,4.951 , -0.150 , 2.690 ,
-4.746 , 1.570 , -2.360 ,-3.933 , 1.480 , -1.880 ,-0.136 , 0.300 , -0.400 ,
6.644 , -2.030 , 2.020 ,-3.813 , 1.710 , -1.560 ,4.617 , 0.290 , 2.910 ,
-6.265 , 2.400 , -2.580 ,4.093 , -1.630 , 0.650 ,4.462 , -1.630 , 0.910 ,
-6.065 , 2.210 , -2.620 ,-3.180 , 2.460 , -0.360 ,0.321 , 0.670 , 0.310 ,
0.651 , 2.890 , 2.740 ,-2.534 , 2.340 , -0.020 ,4.549 , 0.070 , 2.670 ,
3.559 , -2.550 , -0.610 ,0.551 , -0.890 , -1.060 ,-1.518 , 2.220 , 0.580 ,
-3.159 , 1.200 , -1.570 ,-2.007 , -0.220 , -2.170 ,6.525 , -1.550 , 2.460 ,
-2.131 , 1.670 , -0.380 ,2.629 , 1.580 , 2.850 ,4.195 , -0.010 , 2.370 ,
0.036 , 0.520 , 0.000 ,2.045 , -0.250 , 0.640 ,5.973 , -2.280 , 1.370 ,
0.283 , 2.520 , 2.160 ,3.410 , -2.310 , -0.440 ,0.959 , 0.210 , 0.350 ,
-1.575 , 0.990 , -0.640 ,-4.891 , 1.210 , -2.730 ,-2.085 , 1.000 , -0.980 ,
6.005 , -2.060 , 1.630 ,2.386 , -2.910 , -1.730 ,-0.384 , -1.580 , -2.340 ,
1.402 , 2.040 , 2.490 ,5.201 , -2.920 , 0.230 ,0.025 , -1.270 , -1.740 ,
0.240 , 1.910 , 1.560 ,2.189 , 0.970 , 1.990 ,-1.690 , 0.060 , -1.610 ,
3.694 , -2.530 , -0.420 ,-6.706 , 2.200 , -2.980 ,2.977 , -1.910 , -0.300 ,
0.086 , 1.900 , 1.460 ,4.434 , -1.830 , 0.800 ,1.252 , -1.270 , -0.860 ,
5.875 , -0.920 , 2.710 ,-5.691 , 2.550 , -1.910 ,-3.692 , 0.150 , -2.890 ,
-0.027 , -2.210 , -2.670 ,-6.685 , 2.160 , -2.980 ,0.581 , -2.270 , -2.300 ,
-0.439 , 0.910 , 0.140 ,-7.749 , 2.900 , -2.980 ,6.424 , -2.420 , 1.630 ,
6.505 , -2.450 , 1.660 ,-5.059 , 1.130 , -2.850 ,0.944 , -1.340 , -1.110 ,
2.758 , -0.700 , 0.790 ,3.060 , -1.360 , 0.350 ,-2.076 , 2.080 , 0.180 ,
3.632 , -0.940 , 1.170 ,2.179 , -2.810 , -1.690 ,1.048 , 0.610 , 0.910 ,
-5.738 , 2.120 , -2.320 ,-4.998 , 2.950 , -0.980 ,-1.529 , 0.890 , -0.600 ,
-0.287 , -0.240 , -0.850 ,-3.650 , 2.660 , -0.320 ,4.822 , -1.740 , 1.230 ,
0.603 , 1.530 , 1.530 ,-2.157 , 1.570 , -0.350 ,1.955 , -1.610 , -0.630 ,
2.649 , -2.020 , -0.550 ,-7.489 , 2.930 , -2.710 ,7.271 , -2.480 , 2.220 ,
-4.194 , 1.060 , -2.260 ,1.169 , -1.010 , -0.570 ,3.212 , -0.430 , 1.430 ,
-3.850 , 0.920 , -2.150 ,6.591 , -2.430 , 1.810 ,3.675 , 0.700 , 2.880 ,
-2.459 , 1.660 , -0.440 ,1.834 , 0.960 , 1.860 ,3.515 , -0.560 , 1.530 ,
2.769 , -1.300 , 0.280 ,0.287 , -0.930 , -1.080 ,0.991 , 1.400 , 1.720 ,
-3.568 , 1.090 , -1.760 ,3.791 , -0.960 , 1.340 ,-2.818 , -0.080 , -2.390 ,
1.576 , 0.320 , 1.070 ,-3.069 , -0.030 , -2.510 ,-1.056 , 1.480 , 0.390 ,
3.460 , -1.550 , 0.540 ,-0.159 , -1.390 , -1.820 ,-0.812 , 1.300 , 0.390 ,
3.044 , -0.740 , 1.060 ,2.950 , 0.970 , 2.690 ,6.468 , -2.620 , 1.590 ,
0.767 , 0.510 , 0.720 ,0.340 , -0.980 , -1.050 ,2.941 , -0.520 , 1.220 ,
-6.620 , 2.650 , -2.300 ,2.246 , 1.630 , 2.870 ,3.891 , 0.490 , 2.890 ,
-2.236 , 2.910 , 1.020 ,0.024 , 0.710 , 0.420 ,0.870 , 1.860 , 2.150 ,
4.105 , -0.420 , 2.150 ,1.765 , 0.800 , 1.730 ,-2.982 , 2.390 , 0.000 ,
1.925 , -2.380 , -1.300 ,4.688 , -2.630 , 0.380 ,0.306 , 1.280 , 1.200 ,
-4.482 , 2.000 , -1.420 ,0.313 , -0.610 , -0.660 ,1.569 , 0.190 , 1.010 ,
-3.303 , 0.040 , -2.530 ,-3.726 , 2.820 , -0.070 ,-2.748 , 0.250 , -1.930 ,
-3.960 , 2.080 , -0.960 ,4.808 , -2.720 , 0.400 ,-3.534 , 1.330 , -1.400 ,
-2.711 , 0.010 , -2.130 ,-1.058 , -0.560 , -1.540 ,4.204 , -0.740 , 1.950 ,
2.171 , -0.050 , 1.220 ,-0.573 , -1.930 , -2.550 ,3.817 , -2.820 , -0.370 ,
5.038 , -0.520 , 2.760 ,7.688 , -2.550 , 2.600 ,-1.403 , -1.580 , -2.770 ,
-2.575 , 2.920 , 0.870 ,-0.831 , 0.740 , -0.070 ,-6.108 , 2.420 , -2.080 ,
-0.127 , 0.380 , 0.070 ,-0.036 , 1.010 , 0.760 ,-0.871 , 2.050 , 1.210 ,
-0.703 , -1.890 , -2.570 ,6.021 , -2.150 , 1.860 ,2.273 , 0.020 , 1.400 ,
0.625 , -2.490 , -2.230 ,-0.148 , 0.880 , 0.570 ,-1.093 , -0.650 , -1.600 ,
-3.542 , 2.940 , 0.250 ,0.436 , -1.550 , -1.420 ,3.459 , 0.290 , 2.510 ,
6.185 , -2.490 , 1.660 ,5.284 , -2.940 , 0.590 ,0.834 , -1.690 , -1.270 ,
-0.047 , 1.180 , 0.960 ,1.439 , -1.970 , -1.120 ,-7.335 , 2.970 , -2.340 ,
2.785 , -0.680 , 1.100 ,5.435 , -1.760 , 1.880 ,-0.706 , -0.280 , -0.930 ,
-7.361 , 2.360 , -2.950 ,0.275 , 0.590 , 0.620 ,4.306 , -0.260 , 2.590 ,
1.737 , 0.030 , 1.090 ,1.111 , -1.280 , -0.640 ,4.984 , -2.210 , 1.140 ,
0.740 , 2.370 , 2.720 ,1.444 , 0.730 , 1.590 ,-2.544 , 2.190 , 0.260 ,
-2.880 , 0.110 , -2.030 ,-1.624 , 2.800 , 1.510 ,7.518 , -2.960 , 2.180 ,
3.983 , 0.030 , 2.680 ,1.406 , -1.170 , -0.300 ,1.843 , -2.430 , -1.240 ,
-2.162 , -1.200 , -2.810 ,3.664 , -2.010 , 0.450 ,2.492 , 0.570 , 2.190 ,
2.935 , -2.270 , -0.310 ,0.827 , -2.770 , -2.270 ,5.017 , -0.680 , 2.720 ,
-6.975 , 2.130 , -2.850 ,2.504 , 0.800 , 2.440 ,-5.506 , 1.050 , -2.890 ,
4.183 , -1.590 , 1.250 ,0.680 , -0.420 , -0.030 ,5.714 , -1.370 , 2.540 ,
5.823 , -1.550 , 2.440 ,-6.555 , 2.430 , -2.240 ,-4.104 , 2.430 , -0.530 ,
0.873 , 0.480 , 1.010 ,-0.605 , -0.180 , -0.670 ,4.511 , -2.460 , 0.640 ,
-1.089 , 0.590 , -0.240 ,-0.724 , 2.300 , 1.710 ,-1.241 , -0.410 , -1.330 ,
5.895 , -1.980 , 2.090 ,-0.266 , -1.880 , -2.100 ,-1.295 , 1.660 , 0.690 ,
-0.267 , 1.590 , 1.340 ,0.648 , 1.560 , 1.950 ,0.613 , -2.480 , -2.070 ,
-2.912 , -0.770 , -2.830 ,-1.820 , 0.500 , -0.810 ,-1.495 , -1.500 , -2.560 ,
5.622 , -1.410 , 2.490 ,-2.265 , 1.390 , -0.230 ,7.118 , -2.660 , 2.300 ,
-0.053 , -1.940 , -1.980 ,-0.851 , -2.250 , -2.840 ,0.745 , 1.200 , 1.690 ,
-4.689 , 0.940 , -2.350 ,1.838 , -1.090 , 0.190 ,-6.024 , 1.440 , -2.780 ,
-2.782 , 0.640 , -1.310 ,2.522 , -0.610 , 1.150 
};



//******************************************************************************************************//

uint8_t get_key(uint8_t* k)
{
	// Load key here
	return 0x00;
}

#if SS_VER == SS_VER_2_0
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt) // char형값 16개 -> [0~4] 4개만 사용
#endif

{
	/**********************************
	* Start user-specific code here. */
    
    char *p;
    float temp;

    
    // 입력층 
    X[0] = input[count];
    X[1] = input[count+1];
    X[2] = input[count+2];
    count+=3;
   
    float y;


    int i, j;
	float sum = 0;
    


	// Weight 설정 
    WX[0][0] = -2.464;
    WX[0][1] = -4.633;
    WX[0][2] = -0.948;
    WX[0][3] = -4.241;
    WX[0][4] = 0.847;
    WX[0][5] = 1.477;
    WX[1][0] = -3.501;
    WX[1][1] = 1.613;
    WX[1][2] = 1.073;
    WX[1][3] = 0.331;
    WX[1][4] = 4.349;
    WX[1][5] = 4.728;
    WX[2][0] = 3.537;
    WX[2][1] = 0.215;
    WX[2][2] = -3.727;
    WX[2][3] = -0.018;
    WX[2][4] = -3.035;
    WX[2][5] = -4.727;


    WH[0][0] = 3.746;
    WH[0][1] = -4.071;
    WH[1][0] = 4.049;
    WH[1][1] = -1.419;
    WH[2][0] = 3.681;
    WH[2][1] = 2.544;
    WH[3][0] = -1.990;
    WH[3][1] = -4.119;
    WH[4][0] = 4.102;
    WH[4][1] = -2.527;
    WH[5][0] = 1.181;
    WH[5][1] = -3.696;

   
	// 초기화 
	memset(WSX, 0, sizeof(WSX));
	memset(WSH, 0, sizeof(WSH));
    
    

    //y = relu(input[count]);
    //y = sigmoid(input[count]);
    //y = tanh2(input[count]);
    //y = Hardsigmoid(input[count]);
    //y = LeaklyReLU(input[count]);
    //y = ELU(input[count]);
    //y = Softplus(input[count]);
    //y = Softsign(input[count]);
    //y = Swish(input[count]);
    
     //y = relu(in);
    //y = sigmoid(in);
    //y = tanh2(in);
    //y = Hardsigmoid(in);
    //y = LeaklyReLU(in);
    //y = ELU(in);
    //y = Softplus(in);
    //y = Softsign(in);
    //y = Swish(in);
    
//     trigger_low();
//     X[0] = in;
//     in+=0.01; 
    //count += 1;
    
    

    
    trigger_high();


	// 가중합 + 활성함수 
	for (i = 0; i < HIDDEN; i++) 
	{
		for (j = 0; j < INPUT; j++)  
		{
			WSX[i] += X[j] * WX[j][i];
		}
            
		AFR[i] = sigmoid(WSX[i]);  /// 여기 입니다 ///
		//AFR[i] = tanh2(WSX[i]);  /// 여기 입니다 ///
		//AFR[i] = relu(WSX[i]);
        
        //AFR[i] = Hardsigmoid(WSX[i]);
        //AFR[i] = LeaklyReLU(WSX[i]);
        //AFR[i] = ELU(WSX[i]);
        //AFR[i] = Softplus(WSX[i]);
        //AFR[i] = Softsign(WSX[i]);
        //AFR[i] = Swish(WSX[i]);
  
	}




	// 가중합 + softmax exp연산 
	for (i = 0; i < OUTPUT; i++)
	{
		for (j = 0; j < HIDDEN; j++) 
		{
			WSH[i] += AFR[j] * WH[j][i];
		}
		Y[i] = exp(WSH[i]); // 분자 
		sum += Y[i];
	}

	
	for (i = 0; i < OUTPUT; i++) 
	{
		Y[i] /= sum; 
	}
    

      //trigger_low();
    

//trigger_low();
	//16 hex bytes held in 'pt' were sent
	//from the computer. Store your response
	//back into 'pt', which will send 16 bytes
	//back to computer. Can ignore of course if
	//not needed
	
	
   
 
    
//     // 입력값 3개를 더해서 활성함수 입력 만들기 
//     p = X;
//     pt[0] = p[0];
//     pt[1] = p[1];
//     pt[2] = p[2];
//     pt[3] = p[3];
    
//     pt[4] = p[4];
//     pt[5] = p[5];
//     pt[6] = p[6];
//     pt[7] = p[7];
    
//     pt[8] = p[8];
//     pt[9] = p[9];
//     pt[10] = p[10];
//     pt[11] = p[11];
    
//     X[0] = X[0] + X[1] + X[2];
//     pt[12] = p[0];
//     pt[13] = p[1];
//     pt[14] = p[2];
//     pt[15] = p[3];

    
    
    
    /*
    // 출력값 1개 -> softmax라서 나머지 1개 알 수 있음 
	p = Y;
    pt[12] = p[0];
    pt[13] = p[1];
    pt[14] = p[2];
    pt[15] = p[3];
    */
    trigger_low();
    
    
	/* End user-specific code here. *
	********************************/
	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t reset(uint8_t* x)
{
	// Reset key here if needed
	return 0x00;
}


int main(void)
{
    platform_init();
	init_uart();	
	trigger_setup();
	
 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/
		
	simpleserial_init();		
	simpleserial_addcmd('p', 16, get_pt);
#if SS_VER != SS_VER_2_0
	simpleserial_addcmd('k', 16, get_key);
	simpleserial_addcmd('x', 0, reset);
#endif
	while(1)
		simpleserial_get();
}
