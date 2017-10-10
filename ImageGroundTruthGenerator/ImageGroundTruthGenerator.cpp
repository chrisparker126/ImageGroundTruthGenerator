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
#include <boost/filesystem.hpp>



bool isWindowSizeValidForDimension(size_t windowSize, size_t imageSize)
{
	return imageSize % windowSize == 0;
}

bool isStrideSizeValidForDimension(size_t strideSize, size_t windowSize, size_t imageSize)
{
	return (imageSize - windowSize) % strideSize == 0;
}

using namespace cv;
using namespace std;
using namespace boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, const char *argv[])
{
	size_t x_stride, y_stride, window_size_x, window_size_y,
		target_res_x = 0, target_res_y = 0;
	double resize_factor_x = 1.0, resize_factor_y = 1.0;
	std::string fileName , outputFile;
	fs::path inputFilePath;
	try
	{
		options_description desc{ "Options" };
		desc.add_options()
			("help,h", "Help screen")
			("image", value<string>()->required(), "input image file path")
			("annotation", value<string>(), "out annotation file path")
			("stride-x", value<size_t>()->required(), "stride in X dimension")
			("stride-y", value<size_t>()->required(), "stride in Y dimension")
			("window-x", value<size_t>()->required(), "sliding window size in X dimension")
			("window-y", value<size_t>()->required(), "sliding window size in Y dimension")
			("target-res-x", value<size_t>(), "target resolution in x")
			("target-res-y", value<size_t>(), "target resolution in y");

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help"))
			std::cout << desc << '\n';

		if (vm.count("image"))
			fileName = vm["image"].as<std::string>();


		if (vm.count("annotation"))
			outputFile = vm["annotation"].as<std::string>();
		else
		{
			inputFilePath = fileName;
			auto fileNameWithoutExtension = inputFilePath.stem().string();
			outputFile = fileNameWithoutExtension + ".annote";
		}

		if (vm.count("stride-x"))
			x_stride = vm["stride-x"].as<size_t>();

		if (vm.count("stride-y"))
			y_stride = vm["stride-y"].as<size_t>();

		if (vm.count("window-x"))
			window_size_x = vm["window-x"].as<size_t>();

		if (vm.count("window-y"))
			window_size_y = vm["window-y"].as<size_t>();

		
		if (vm.count("target-res-x") && vm.count("target-res-y"))
		{
			target_res_x = vm["target-res-x"].as<size_t>();
			target_res_y = vm["target-res-y"].as<size_t>();
		}
	}
	catch (const boost::program_options::error &ex)
	{
		std::cerr << ex.what() << '\n';
		return 1;
	}

	Mat image = imread(fileName, 1);

	// Convert it to gray scale
	cvtColor(image, image, CV_RGB2GRAY);

	size_t imageSize_x = image.cols, imageSize_y = image.rows;

	// determinig resize factor
	if (target_res_x != 0)
		resize_factor_x = imageSize_x / (double) target_res_x;

	if(target_res_y != 0)
		resize_factor_y = imageSize_y / (double) target_res_y;

	cout << "resizing image by x_factor: " << resize_factor_x << " and y_factor: " << resize_factor_y << endl;

	resize(image, image, Size(), 1./resize_factor_x, 1./resize_factor_y, cv::INTER_LANCZOS4);

	imageSize_x = image.cols;
	imageSize_y = image.rows;

	cout << "actual target res: " << imageSize_x << " x " << imageSize_y << endl;

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

	size_t stride_index_x_max = (imageSize_x - window_size_x) / x_stride + 1;
	size_t stride_index_y_max = (imageSize_y - window_size_y) / y_stride + 1;
	namedWindow("small Image", CV_WINDOW_AUTOSIZE);

	std::ofstream annotation(outputFile.c_str(), std::ios::binary);

	auto srcImageFileName = outputFile + ".PNG";
	// first file name
	annotation << srcImageFileName << endl;

	//  window x size, and window y size
	annotation << window_size_x << "," << window_size_y << endl;

	for (size_t j = 0; j < stride_index_y_max; j++) {
		for (size_t i = 0; i < stride_index_x_max; i++) {
			auto xPos = i*x_stride, yPos = j*y_stride;
			cv::Mat smallImage = cv::Mat(image, cv::Rect(xPos, yPos, 
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
				return 1;
			}

			annotation << xPos << "," << yPos << "," << input << endl;
		}
	}

	annotation.close();
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(3);
	imwrite(srcImageFileName, image, compression_params);
	return 0;
}

