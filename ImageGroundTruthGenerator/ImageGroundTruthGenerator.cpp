// ImageTrainingCutouts.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

#define IMAGE_FOLDER R"(C:\Users\crisyp\Documents\machine_learning\tennis_training_data\frame_150.PNG)"
#define DEST_FOLDER R"(C:\Users\crisyp\Documents\machine_learning\tennis_cutouts\)"
#define M_SIZE 2
#define N_SIZE 2
int main()
{
	Mat image;

	image = imread(IMAGE_FOLDER, 1);
	if (image.empty())
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	/// Convert it to gray
	cvtColor(image, image, CV_RGB2GRAY);
	//resize(image,image,Size(0,0),0.5,0.5,INTER_LINEAR);
	//namedWindow("Image", CV_WINDOW_AUTOSIZE);
	//imshow("Image", image);

	// get the image data
	int height = image.rows;
	int width = image.cols;
	int cutOutHeight = height / M_SIZE;
	int cutOutWidth = width / N_SIZE;
	cv::Mat smallImage = cv::Mat(image, cv::Rect(0, 0, 110, 70));

	//namedWindow("small Image", CV_WINDOW_AUTOSIZE);
	//imshow("small Image", smallImage);


	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(3);

	for (int i = 0; i < M_SIZE; i++)
	{
		for (int j = 0; j < N_SIZE; j++)
		{
			std::ostringstream ostr;
			ostr << "origin_" << i*cutOutHeight << "_" << j*cutOutWidth << ".png";
			cv::Mat smallImage = cv::Mat(image, cv::Rect(j*cutOutWidth, i*cutOutHeight, cutOutWidth, cutOutHeight));

			Mat downSampledImage(cv::Size(cutOutWidth / 10, cutOutHeight / 10), CV_32F);
			pyrDown(smallImage, downSampledImage, cv::Size(cutOutWidth / 10, cutOutHeight / 10));

			imwrite(DEST_FOLDER + ostr.str(), smallImage, compression_params);
			imwrite(DEST_FOLDER + std::string("ds_") + ostr.str(), downSampledImage, compression_params);

		}
	}
	waitKey(30);


	return 0;
}

