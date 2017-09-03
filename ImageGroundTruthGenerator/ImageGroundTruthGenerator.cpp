// ImageTrainingCutouts.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <boost\program_options.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <boost/property_tree/ptree.hpp>



bool isWindowSizeValidForDimension(size_t windowSize, size_t imageSize)
{
	return imageSize % windowSize == 0;
}

bool isStrideSizeValidForDimension(size_t strideSize, size_t windowSize, size_t imageSize)
{
	return (imageSize - windowSize) % strideSize == 0;
}

#define IMAGE_FILE R"(C:\Users\crisyp\Documents\machine_learning\tennis_training_data\frame_150.PNG)"

using namespace cv;
using namespace std;
using namespace boost::program_options;

int main(int argc, const char *argv[])
{
	size_t x_stride = 40, y_stride = 60, window_size_x = 80, window_size_y = 120;
	std::string file = IMAGE_FILE;
	try
	{
		options_description desc{ "Options" };
		desc.add_options()
			("help,h", "Help screen")
			("image", value<string>(), "input image file path")
			("annotation", value<string>(), "out annotation file path")
			("stride-x", value<size_t>(), "stride in X dimension")
			("stride-y", value<size_t>(), "stride in Y dimension")
			("window-x", value<size_t>(), "sliding window size in X dimension")
			("window-y", value<size_t>(), "sliding window size in Y dimension");

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help"))
			std::cout << desc << '\n';
		else if (vm.count("age"))
			std::cout << "Age: " << vm["age"].as<int>() << '\n';
		else if (vm.count("pi"))
			std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
		else if (vm.count("name"))
			std::cout << "Name: " << vm["name"].as<std::string>() << '\n';

	}
	catch (const boost::program_options::error &ex)
	{
		std::cerr << ex.what() << '\n';
	}


	Mat image = imread(file, 1);

	/// Convert it to gray
	cvtColor(image, image, CV_RGB2GRAY);

	size_t imageSize_x = image.cols, imageSize_y = image.rows;

	// check a sliding window can actually be made with the image Size 
	if (!isWindowSizeValidForDimension(window_size_x, imageSize_x) && !isWindowSizeValidForDimension(window_size_y, imageSize_x))
	{
		std::cout << "Window size not valid!" << std::endl;
		return 1;
	}

	// now check stride size is valid for x and y dimension
	if (!isStrideSizeValidForDimension(x_stride, window_size_x, imageSize_x) && !isStrideSizeValidForDimension(y_stride, window_size_y, imageSize_y))
	{
		std::cout << "stride size not valid!" << std::endl;
		return 1;
	}

	// we essentially want to iterate through each image window and display to the user whether it valid or not
	// output in first line the dimension

	size_t window_index_x_max = imageSize_x / window_size_x;
	size_t window_index_y_max = imageSize_y / window_size_y;

	size_t stride_index_x_max = (imageSize_x - window_size_x) / x_stride + 1;
	size_t stride_index_y_max = (imageSize_y - window_size_y) / y_stride + 1;
	namedWindow("small Image", CV_WINDOW_AUTOSIZE);

	for (size_t j = 0; j < stride_index_y_max; j++) {
		for (size_t i = 0; i < stride_index_x_max; i++) {

			cv::Mat smallImage = cv::Mat(image, cv::Rect(i*x_stride, j*y_stride, 
				window_size_x, window_size_y));
			imshow("small Image", smallImage);
			waitKey(30);
			size_t input; 
			cin >> input;

			if (input != 0 && input != 1)
			{
				std::cout << "You are only allowed boolean inputs for ground truth!" << endl;
				std::cout << "Press key to continue..." << endl;
				std::string p;
				cin >> p;
				return;
			}

			cout << "input: " << input << endl;
		}
	}

	return 0;
}

