#include <opencv2/opencv.hpp> 
#include "util.h"

void DIBR(IplImage *color, IplImage *depth, IplImage *dibr_n);

void DIBR_shift_sensor(IplImage *color, IplImage *depth, IplImage *dibr_n, 
		const float f, const float tc, const float Zc, const float s);

void pyramid_transform(IplImage *dibr, const int level);