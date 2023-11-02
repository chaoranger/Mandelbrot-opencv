#include <iostream>
#include <time.h>
#include <windows.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#define width 800
#define height 600
#define resolution width * height
int main()
{
	//��ֵ����
	bool skippable = 0;
	bool debug = 1;
	int ch;
	int max = 10;
	unsigned int iteration = 50;
	unsigned short* result = (unsigned short*)malloc(sizeof(unsigned short) * resolution);
	double centre[2] = { 0.0,0.0 };
	double step = 4.0 / width;
	double* z_re = (double*)malloc(sizeof(double*) * resolution);
	double* z_im = (double*)malloc(sizeof(double*) * resolution);
	double* c_re = (double*)malloc(sizeof(double*) * resolution);
	double* c_im = (double*)malloc(sizeof(double*) * resolution);
	double* temp = (double*)malloc(sizeof(double*) * resolution);
	double delta_time = 0;
	int times = 0;
	if (debug == 1)
	{
		system("cls");
		std::cout << "�ѿ���debug" << std::endl << "Ĭ�Ϸֱ��ʣ�" << height << "x" << width << std::endl << "3�������" << std::endl;
		Sleep(3000);
	}
	cv::namedWindow("image", cv::WindowFlags::WINDOW_FULLSCREEN);
	system("cls");
	while (true)
	{
		if (skippable == 0)
		{
			//���ó�ֵ
			delta_time = clock();
#pragma omp parallel for
			for (int i = 0; i < resolution; i++)
			{
				z_re[i] = 0;
				z_im[i] = 0;
				c_re[i] = step * (i % width - width / 2) + centre[0];
				c_im[i] = step * (height / 2 - i / width) + centre[1];
				result[i] = 0;
			}
			delta_time = clock() - delta_time;
			std::cout << "���ó�ֵ��t=" << delta_time << ';';
			//����
			delta_time = clock();
#pragma omp parallel for
			for (int j = 0; j < resolution; j++)
			{
				for (int i = 0; i < iteration; i++)
				{
					if (result[j] <= i)
					{
						temp[j] = z_re[j] * z_re[j] - z_im[j] * z_im[j] + c_re[j];
						z_im[j] = 2 * z_re[j] * z_im[j] + c_im[j];
						z_re[j] = temp[j];
						if (z_re[j] * z_re[j] + z_im[j] * z_im[j] <= 4)
							result[j]++;
					}
				}
			}
#pragma omp parallel for
			for (int i = 0; i < resolution; i++)
			{
				if (result[i] == iteration)
					result[i] = 0;
				else
					result[i] *= 65535 / iteration;
			}
			delta_time = clock() - delta_time;
			std::cout << "������t=" << delta_time << ';';
			//ͼ����ʾ
			delta_time = clock();
			cv::Mat image(height, width, CV_16UC1, result);
			cv::imshow("image", image);
			delta_time = clock() - delta_time;
			std::cout << "ͼ����ʾ��t=" << delta_time << ";�Ŵ��ʣ�" << 1.0/step << std::endl;
		}
		//��ֵ���
		ch = cv::waitKey(0);    //�ڼ��������ַ�ǰ�����ڵȴ�״̬;��ȡ��ֵ
		if (ch == 27) { break; }//�˳�
		else if (ch == 97)	{ centre[0] -= 0.01 * width * step;	}//����:a
		else if (ch == 100) { centre[0] += 0.01 * width * step;	}//����:d
		else if (ch == 115) { centre[1] -= 0.01 * width * step;	}//����:s
		else if (ch == 119) { centre[1] += 0.01 * width * step;	}//����:w
		else if (ch == 113) { step *= 0.95;	}//�Ŵ�:q
		else if (ch == 101) { step *= 1.05; }//��С:e
		else { skippable = 1; }
	}
	//�رճ���
	cv::destroyWindow("image");
	free(z_re);
	free(z_im);
	free(c_re);
	free(c_im);
	free(temp);
	free(result);
	return 0;
}
