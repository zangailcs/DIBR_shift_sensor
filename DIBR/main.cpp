#include "opencv2/opencv.hpp" 
#include "DIBR.h" 
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	//IplImage *input_image = cvLoadImage("image00300.jpg",1);
	//IplImage *depthmap = cvLoadImage("depth00300.jpg",0);

	int img_num = 100;
	char colorImageName[50] = { "0" };
	char depthImageName[50] = { "0" };

	for (int i = 0; i < img_num; i++) {
		cout << "===== frame " << i << " =====" << endl;
		sprintf(colorImageName, "cam0_bd\\color-cam0-f%03d.jpg", i);
		sprintf(depthImageName, "cam0_bd\\depth-cam0-f%03d.png", i);
		IplImage *input_image = cvLoadImage(colorImageName, 1);
		IplImage *depthmap = cvLoadImage(depthImageName, 0);

		IplImage *dibr_L = cvCreateImage(cvGetSize(input_image), 8, 3);
		IplImage *dibr_R = cvCreateImage(cvGetSize(input_image), 8, 3);
		//DIBR(input_image, depthmap, dibr);
		float f = 1885.f, tc = 0.065f, Zc = 200.f, s = -2.0f;

		DIBR_shift_sensor(input_image, depthmap, dibr_L, f, tc, Zc, s);
		//cvShowImage("dibr",dibr); 
		cv::imwrite("output_holes_bd\\holes-" + std::to_string(i) + "_L.jpg", cv::Mat(dibr_L));
		pyramid_transform(dibr_L, 5);
		cv::imwrite("output_bd\\final-" + std::to_string(i) + "_L.jpg", cv::Mat(dibr_L));

		s = -s;
		DIBR_shift_sensor(input_image, depthmap, dibr_R, f, tc, Zc, s);
		//cvShowImage("dibr",dibr); 
		cv::imwrite("output_holes_bd\\holes-" + std::to_string(i) + "_R.jpg", cv::Mat(dibr_R));
		pyramid_transform(dibr_R, 5);
		cv::imwrite("output_bd\\final-" + std::to_string(i) + "_R.jpg", cv::Mat(dibr_R));
		
		cvReleaseImage(&input_image);
		cvReleaseImage(&depthmap);
		cvReleaseImage(&dibr_L);
		cvReleaseImage(&dibr_R);
	}

	//cvWaitKey(0);
	return 0;
}