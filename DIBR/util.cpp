#include "util.h"
void depthpreprocess(IplImage *depth)  //深度图预处理
{
	int imageHeight = depth->height, imageWidth = depth->width;

	cv::Mat srcDepth(depth);

	int **dir = new int*[imageHeight];
	for (int i = 0; i < imageHeight; i++)
	{
		dir[i] = new int[imageWidth]();
	}

	for (int j = 0; j < srcDepth.rows; j++)//先水平方向上判断需要膨胀的像素
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{


			if (i + 1 >= 0 && i + 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i + 1) >= 5)//和右面像素比
			{
				dir[j][i] = 1;
			}

			if (i - 1 >= 0 && i - 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i - 1) >= 5)
			{
				dir[j][i] = -1;
			}
		}
	}
	//水平方向上判断完后开始膨胀
	int i_;
	for (int j = 0; j < srcDepth.rows; j++)
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{
			if (dir[j][i] != 0)
			{
				for (int k = 1; k < 2; k++)  //膨胀两个像素
				{
					//j_ = j + dir[j][i] * k;
					i_ = i + dir[j][i] * k;
					//if (j_ <= 767 && j_ >= 0)
					//	srcDepth.at<uchar>(j_, i) = srcDepth.at<uchar>(j, i);
					if (i_ < imageWidth && i_ >= 0)
						srcDepth.at<uchar>(j, i_) = srcDepth.at<uchar>(j, i);
				}

			}
		}
	}

	for (int j = 0; j < srcDepth.rows; j++) // 垂直方向上判断需要膨胀的像素
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{

			if (j + 1 >= 0 && j + 1 < imageHeight && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j + 1, i) >= 5) //和下面像素比					
			{
				dir[j][i] = 1;
			}

			if (j - 1 >= 0 && j - 1 < imageHeight && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j - 1, i) >= 5) //和上面像素比					
			{
				dir[j][i] = -1;
			}

		}
	}
	//垂直方向上判断完后开始膨胀
	int j_;
	for (int j = 0; j < srcDepth.rows; j++)
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{
			if (dir[j][i] != 0)
			{
				for (int k = 1; k < 2; k++)  //膨胀两个像素
				{
					j_ = j + dir[j][i] * k;
					//	i_ = i + dir[j][i] * k;
					if (j_ < imageHeight && j_ >= 0)
						srcDepth.at<uchar>(j_, i) = srcDepth.at<uchar>(j, i);
					//	if (i_ <= 1023 && i_ >= 0)
					//	   srcDepth.at<uchar>(j, i_) = srcDepth.at<uchar>(j, i);
				}

			}
		}
	}
	/*对上面膨胀的图像前景与背景深度变化差异大的像素进行中值滤波，
	沿水平方向从左到右扫描深度图像，计算水平方向相邻两个像素点之间的深度差的绝对值，并将该绝对值与一个预先设定的阈值砌进行比较，如果该
	深度差的绝对值大于阈值，则将该深度值突变像素点标记为滤波区域，重复上述过程，最后再采用3×3的单位掩膜对该区域所有像素点进行中值滤波。*/

	for (int j = 0; j < srcDepth.rows; j++)//先水平方向上判断滤波区域,dir标记为2
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{


			if (i + 1 >= 0 && i + 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i + 1) >= 5)//和右面像素比
			{
				dir[j][i] = 2;
			}

			if (i - 1 >= 0 && i - 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i - 1) >= 5)//和左边像素比
			{
				dir[j][i] = 2;
			}
		}
	}

	//尝试高斯平滑，和中值滤波比较
	cv::Mat dst = cv::Mat(srcDepth.rows, srcDepth.cols, CV_8UC1, cv::Scalar(0));
	int templates[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
	//memcpy(dst,srcDepth,srcDepth.rows*srcDepth.cols*sizeof());
	for (int j = 1; j < srcDepth.rows - 1; j++)
	{
		for (int i = 1; i < srcDepth.cols - 1; i++)
		{
			if (dir[j][i] == 2)
			{
				int sum = 0;
				int index = 0;
				for (int m = -1; m < 2; m++)
				{
					for (int n = -1; n < 2; n++)
						sum += srcDepth.at<uchar>(j + m, i + n)*templates[index++];

				}
				sum /= 16;
				if (sum > 255)
					sum = 255;
				dst.at<uchar>(j, i) = sum;
			}
			else
				dst.at<uchar>(j, i) = srcDepth.at<uchar>(j, i);


		}
	}
	srcDepth = dst.clone();
}