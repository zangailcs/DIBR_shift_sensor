#include "DIBR.h" 

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


void DIBR_shift_sensor(IplImage *color, IplImage *depth, IplImage *dibr_n)
{
	depthpreprocess(depth);



}