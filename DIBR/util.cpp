#include "util.h"
void depthpreprocess(IplImage *depth)  //���ͼԤ����
{
	int imageHeight = depth->height, imageWidth = depth->width;

	cv::Mat srcDepth(depth);

	int **dir = new int*[imageHeight];
	for (int i = 0; i < imageHeight; i++)
	{
		dir[i] = new int[imageWidth]();
	}

	for (int j = 0; j < srcDepth.rows; j++)//��ˮƽ�������ж���Ҫ���͵�����
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{


			if (i + 1 >= 0 && i + 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i + 1) >= 5)//���������ر�
			{
				dir[j][i] = 1;
			}

			if (i - 1 >= 0 && i - 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i - 1) >= 5)
			{
				dir[j][i] = -1;
			}
		}
	}
	//ˮƽ�������ж����ʼ����
	int i_;
	for (int j = 0; j < srcDepth.rows; j++)
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{
			if (dir[j][i] != 0)
			{
				for (int k = 1; k < 2; k++)  //������������
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

	for (int j = 0; j < srcDepth.rows; j++) // ��ֱ�������ж���Ҫ���͵�����
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{

			if (j + 1 >= 0 && j + 1 < imageHeight && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j + 1, i) >= 5) //���������ر�					
			{
				dir[j][i] = 1;
			}

			if (j - 1 >= 0 && j - 1 < imageHeight && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j - 1, i) >= 5) //���������ر�					
			{
				dir[j][i] = -1;
			}

		}
	}
	//��ֱ�������ж����ʼ����
	int j_;
	for (int j = 0; j < srcDepth.rows; j++)
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{
			if (dir[j][i] != 0)
			{
				for (int k = 1; k < 2; k++)  //������������
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
	/*���������͵�ͼ��ǰ���뱳����ȱ仯���������ؽ�����ֵ�˲���
	��ˮƽ���������ɨ�����ͼ�񣬼���ˮƽ���������������ص�֮�����Ȳ�ľ���ֵ�������þ���ֵ��һ��Ԥ���趨����ֵ�����бȽϣ������
	��Ȳ�ľ���ֵ������ֵ���򽫸����ֵͻ�����ص���Ϊ�˲������ظ��������̣�����ٲ���3��3�ĵ�λ��Ĥ�Ը������������ص������ֵ�˲���*/

	for (int j = 0; j < srcDepth.rows; j++)//��ˮƽ�������ж��˲�����,dir���Ϊ2
	{
		for (int i = 0; i < srcDepth.cols; i++)
		{


			if (i + 1 >= 0 && i + 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i + 1) >= 5)//���������ر�
			{
				dir[j][i] = 2;
			}

			if (i - 1 >= 0 && i - 1 < imageWidth && srcDepth.at<uchar>(j, i) - srcDepth.at<uchar>(j, i - 1) >= 5)//��������ر�
			{
				dir[j][i] = 2;
			}
		}
	}

	//���Ը�˹ƽ��������ֵ�˲��Ƚ�
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