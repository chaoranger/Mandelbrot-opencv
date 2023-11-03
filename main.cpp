#include <iostream>
#include <time.h>
#include <windows.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#define width 640
#define height 480
#define resolution width * height
int main()
{
	//初值设置
	bool skippable = 0;
	bool debug = 1;
	int ch;
	int max = 10;
	int iteration = 100;
	unsigned short* result = (unsigned short*)malloc(sizeof(unsigned short) * resolution);
	double centre[2] = { 0.0,0.0 };
	double step = 4.0 / width;
	double* z_re = (double*)malloc(sizeof(double*) * resolution);
	double* z_im = (double*)malloc(sizeof(double*) * resolution);
	double* c_re = (double*)malloc(sizeof(double*) * resolution);
	double* c_im = (double*)malloc(sizeof(double*) * resolution);
	double* temp = (double*)malloc(sizeof(double*) * resolution);
	//空指针判断
	if (z_re == NULL || z_im == NULL || c_re == NULL || c_im == NULL || temp == NULL || result == NULL)
	{
		std::cerr << "初始化错误：使用malloc申请内存时返回空指针，可能是因为内存空间不足" << std::endl;
		return -1;
	}
	double delta_time = 0;
	int times = 0;
	if (debug == 1)
	{
		std::cout << "已开启debug" << std::endl << "默认分辨率：" << height << "x" << width << std::endl << "3秒后启动" << std::endl;
		Sleep(3000);
	}
	cv::namedWindow("image", cv::WindowFlags::WINDOW_AUTOSIZE);
	//system("cls");
	for (int k = 1; ; k++)
	{
		if (skippable == 0)
		{
			//重置初值
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
			//迭代
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
						if (z_re[j] * z_re[j] + z_im[j] * z_im[j] <= 65536)
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
			//图像显示
			cv::Mat image(height, width, CV_16UC1, result);
			cv::imshow("image", image);
			delta_time = clock() - delta_time;
			std::clog << "[info]第" << k << "次耗时Δt=" << delta_time << "，放大倍率：" << 2.0 / (step * width) << std::endl;
		}
		//键值检测
		ch = cv::waitKey(0);    //在键盘敲入字符前程序处于等待状态;获取键值
		if (ch == 27) { break; }//退出
		else if (ch == 97)	{ centre[0] -= 0.01 * width * step;	}//向左:a
		else if (ch == 100) { centre[0] += 0.01 * width * step;	}//向右:d
		else if (ch == 115) { centre[1] -= 0.01 * width * step;	}//向下:s
		else if (ch == 119) { centre[1] += 0.01 * width * step;	}//向上:w
		else if (ch == 113) { step *= 0.95;	}//放大:q
		else if (ch == 101) { step *= 1.05; }//缩小:e
		else { skippable = 1; }
	}
	//关闭程序
	cv::destroyWindow("image");
	free(z_re);
	free(z_im);
	free(c_re);
	free(c_im);
	free(temp);
	free(result);
	return 0;
}
