#include "DIBR.h" 
#include <cmath>

void DIBR(IplImage *color,IplImage *depth,IplImage *dibr_n)//generate right image and fill holes 
{ 
	depthpreprocess(depth);

	IplImage *gray_b,*gray_g,*gray_r; 
	IplImage *dibr_b,*dibr_g,*dibr_r; 
	gray_b=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1); 
	gray_g=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1); 
	gray_r=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1); 

	dibr_b=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1); 
	cvSetZero(dibr_b);
	dibr_g=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1);  
	cvSetZero(dibr_g);
	dibr_r=cvCreateImage(cvGetSize(depth),IPL_DEPTH_8U,1);
	cvSetZero(dibr_r);
	cvSplit(color,gray_b,gray_g,gray_r,NULL);   

	int table[256];  
	int knear = 0; // knear = 0 means everything is displayed behind the screen 
	int kfar = 128; 
	float xb = 6.5; // eye seperation distance 6.5cm 
	int D = 800; // view distance 3m  
	int Npix = 320; // standard definition display to reduce the parallax (maybe 720 better!!) 
	for (int i = 0; i < 256; i++)  
	{ 
		double A = i * ( knear/64 + kfar/16 ) / 255; 
		double h = - xb * Npix * ( A-kfar/16) / D; 
		table[i] = (int)(h/2); 
	} 
	int S = 25; // depth=0, maxmium shift. !!If Npix changes, this value will change!! 
	int step = depth->widthStep/sizeof(uchar); 
	uchar* datadepth = (uchar*)depth->imageData; 
	uchar* datagray_b = (uchar*)gray_b->imageData; 
	uchar* datagray_g = (uchar*)gray_g->imageData; 
	uchar* datagray_r = (uchar*)gray_r->imageData; 
	uchar* datadibr_b = (uchar*)dibr_b->imageData;  
	uchar* datadibr_g = (uchar*)dibr_g->imageData; 
	uchar* datadibr_r = (uchar*)dibr_r->imageData; 

	//generate right image from color image and associated depth 
	for (int i=0; i<depth->height;i++) 
	{ 
		for (int j=0; j<depth->width; j++) 
		{ 
			int d = (int)(datadepth[i*step+j]); 
			int shift = table[d]; 
			if (j+shift-S>=0) 
			{ 
				datadibr_b[i*step+j+shift-S] = datagray_b[i*step+j]; 
			}   
		} 
	} 
	for (int i=0; i<depth->height;i++) 
	{ 
		for (int j=0; j<depth->width; j++) 
		{ 
			int d = (int)(datadepth[i*step+j]); 
			int shift = table[d]; 
			if (j+shift-S>=0) 
			{ 
				datadibr_g[i*step+j+shift-S] = datagray_g[i*step+j]; 
			} 
		}  
	} 
	for (int i=0; i<depth->height;i++)
	{ 
		for (int j=0; j<depth->width; j++) 
		{ 
			int d = (int)(datadepth[i*step+j]); 
			int shift = table[d]; 
			if (j+shift-S>=0) 
			{ 
				datadibr_r[i*step+j+shift-S] = datagray_r[i*step+j]; 
			} 
		} 
	} 

	cvMerge(dibr_b,dibr_g,dibr_r,NULL,dibr_n); 
} 


void  DIBR_shift_sensor(IplImage *color, IplImage *depth, IplImage *dibr_n,
	const float f, const float tc, const float Zc, const float s)
{
	depthpreprocess(depth);

	IplImage *gray_b, *gray_g, *gray_r;
	IplImage *dibr_b, *dibr_g, *dibr_r;
	gray_b = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
	gray_g = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
	gray_r = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);

	dibr_b = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
	cvSetZero(dibr_b);
	dibr_g = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
	cvSetZero(dibr_g);
	dibr_r = cvCreateImage(cvGetSize(depth), IPL_DEPTH_8U, 1);
	cvSetZero(dibr_r);
	cvSplit(color, gray_b, gray_g, gray_r, NULL);

	float tx = s * tc / 2.f;
	float h = -tx * f / Zc;
	
	int shift_table[256];
	shift_table[0] = round(f * tx / 0.5f + h);
	for (int i = 1; i < 256; i++) {
		shift_table[i] = round(f * tx / i + h);
	}

	int step = depth->widthStep / sizeof(uchar);
	uchar* datadepth = (uchar*)depth->imageData;
	uchar* datagray_b = (uchar*)gray_b->imageData;
	uchar* datagray_g = (uchar*)gray_g->imageData;
	uchar* datagray_r = (uchar*)gray_r->imageData;
	uchar* datadibr_b = (uchar*)dibr_b->imageData;
	uchar* datadibr_g = (uchar*)dibr_g->imageData;
	uchar* datadibr_r = (uchar*)dibr_r->imageData;

	for (int i = 0; i < depth->height; i ++)
	{
		for (int j = 0; j < depth->width; j++)
		{
			int d = (int)(datadepth[i*step + j]);
			int j_v = j + shift_table[d];
			if (j_v >= 0 && j_v < depth->width) {
				int idx = j_v + i * step;
				datadibr_r[idx] = datagray_r[i*step + j];
				datadibr_g[idx] = datagray_g[i*step + j];
				datadibr_b[idx] = datagray_b[i*step + j];
			}
		}
	}

	cvMerge(dibr_b, dibr_g, dibr_r, NULL, dibr_n);

	cvReleaseImage(&gray_b);
	cvReleaseImage(&gray_g);
	cvReleaseImage(&gray_r);
	cvReleaseImage(&dibr_b);
	cvReleaseImage(&dibr_g);
	cvReleaseImage(&dibr_r);
}

bool is_hole(cv::Vec3b point) {
	for (int i = 0; i < 3; i++) {
		if (point[i] != 0) {
			return false;
		}
	}
	return true;
}

void fix_holes_refer_L(cv::Mat &G, const cv::Mat &L)
{
	for (int row = 0; row < G.rows; row++) {
		for (int col = 0; col < G.cols; col++) {
			if (is_hole(G.at<cv::Vec3b>(row, col)) ) {
				//if (is_hole(G.at<cv::Vec3b>(row, col)) && !is_hole(L.at<cv::Vec3b>(row, col))) {
				G.at<cv::Vec3b>(row, col) = L.at<cv::Vec3b>(row, col);
			}
		}
	}
}

void pyramid_transform(IplImage *dibr, const int level)
{
	cv::Mat G0(dibr);
	std::vector<cv::Mat> Gs(level);
	Gs[0] = G0;
	for (int i = 1; i < level; i++) {
		cv::pyrDown(Gs[i - 1], Gs[i], cv::Size(Gs[i - 1].cols / 2, Gs[i - 1].rows / 2));
	}

	for (int i = level - 1; i > 0; i--) {
		cv::Mat L;
		cv::pyrUp(Gs[i], L, cv::Size(Gs[i].cols * 2, Gs[i].rows * 2));
		fix_holes_refer_L(Gs[i - 1], L);
	}

	for (int i = 1; i < level; i++) {
		Gs[i].release();
	}
	//dibr = &IplImage(Gs[0]);
}