#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <time.h>
#include <limits>
using namespace cv;

int maxIter = 0;

void threadFunction(Mat & mandelbrotImg, double scaleX, double scaleY, double x1, double y1,int start,int end)
{
    for (int y = start; y <= end; y++)
    {
        for (int x = 0; x < mandelbrotImg.cols; x++)
        {
            double x0 = x / scaleX + x1;
            double y0 = y / scaleY + y1;

            std::complex<double> z0(x0, y0);
            std::complex<double> z = z0;
            int value = maxIter;
            for (int i = 0; i < maxIter; i++)
            {
                if (z.real() * z.real() + z.imag() * z.imag() > 4.0f)
                {
                    value = i;
                    break;
                }
                else
                {
                    z = z * z + z0;
                }
            }
            if (maxIter - value == 0)
            {
                value = 0;
            }
            else
            {
                value = cvRound(sqrt(value / (double)maxIter) * 255);
            }
            
            int val1 = value / 255;
            int val2 = value % 255;
            mandelbrotImg.at<Vec3b>(y, x)[0] = saturate_cast<uchar>(val1);
            mandelbrotImg.at<Vec3b>(y, x)[1] = saturate_cast<uchar>(val2);
            mandelbrotImg.at<Vec3b>(y, x)[2] = saturate_cast<uchar>(value*2);
        }
    }
}


int main()
{
    Mat mandelbrotImg(1000, 1000, CV_8UC3);
    double x1 = -2.0, x2 = 2.0;
    double y1 = -2.0, y2 = 2.0;

    long double viewPosX = 0;
    long double viewPosY = 0;

    double offSetValue = 0.1;
    while (true)
    {
        std::vector<std::thread> threadVec;
        double scaleX = mandelbrotImg.cols / (x2 - x1);
        double scaleY = mandelbrotImg.rows / (y2 - y1);

        int start = 0;
        int imgRange = mandelbrotImg.size().height / std::thread::hardware_concurrency();
        int end = start + imgRange - 1;

        for (int i = 0; i < (int)std::thread::hardware_concurrency(); i++)
        {
            threadVec.push_back(std::thread(threadFunction, std::ref(mandelbrotImg), scaleX, scaleY, x1+viewPosX, y1+viewPosY, start, end));
            if (i == std::thread::hardware_concurrency() - 2)
            {
                end = mandelbrotImg.size().height - 1;
            }
            else
            {
                end += imgRange;
            }
            start += imgRange;
        }
        for (int i = 0; i < (int)std::thread::hardware_concurrency(); i++)
        {
            threadVec[i].join();
        }
        imshow("image", mandelbrotImg);

        long double val = 0.1;

        while (x2 < val)
        {
            val = val * 0.1;
        }
        offSetValue = val;

        // Poll Event
        char c = (char)cv::waitKey(0);
        //Add iterations
        if (c == 49)
        {
            maxIter++;
        }
        else if (c == 50)
        {
            maxIter += 10;
        }
        else if (c == 51)
        {
            maxIter += 100;
        }
        else if (c == 52)
        {
            maxIter += 1000;
        }
        //Move on set
        else if (c == 97) //A
        {
            viewPosX = viewPosX - offSetValue;
        }
        else if (c == 100) //D
        {
            viewPosX = viewPosX + offSetValue;
        }
        else if (c == 119) //w
        {
            viewPosY = viewPosY - offSetValue;
        }
        else if (c == 115) //s
        {
            viewPosY = viewPosY + offSetValue;
        }
        //Zoom in/out
        else if ( c == 43)
        {
            x1 += offSetValue;
            x2 -= offSetValue;
            y1 += offSetValue;
            y2 -= offSetValue;
        }
        else if ( c == 45)
        {
            x1 -= offSetValue;
            x2 += offSetValue;
            y1 -= offSetValue;
            y2 += offSetValue;
        }
    }
    return 0;
}