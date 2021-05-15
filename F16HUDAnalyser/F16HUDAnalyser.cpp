#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <array>
#include <numeric>
#include <windows.h>

#include "AltimeterTracker.h"
#include "AirspeedIndicatorTracker.h"

int tmp_main()
{
	// open mp4
	std::string filepath = "..\\movie.mp4";
	cv::VideoCapture video;
	video.open(filepath);
	if (!video.isOpened())
	{
		std::cout << "Failed to open " << filepath << std::endl;
		return -1;
	}
	video.set(cv::CAP_PROP_POS_FRAMES, 2060);

	int altimeter_mask_offset = 0;
	int airspeed_mask_offset = 0;
	cv::Mat frame;
	AltimeterTracker altimeter_tracker;
	AirspeedIndicatorTracker airspeed_tracker;

	while (1)
	{
		// load frame
		video >> frame;
		if (frame.empty())
			break;

		cv::Mat airspeed_indicator;
		frame(cv::Rect(cv::Point(59, 59), cv::Point(74, 142))).copyTo(airspeed_indicator);

		cv::cvtColor(airspeed_indicator, airspeed_indicator, cv::COLOR_BGR2GRAY);
		cv::Canny(airspeed_indicator, airspeed_indicator, 100, 200);
		cv::imshow("airspeed indicator", airspeed_indicator);
		cv::waitKey(0);
	}
}

int main()
{
	// open mp4
	std::string filepath = "..\\movie.mp4";
	cv::VideoCapture video;
	video.open(filepath);
	if (!video.isOpened())
	{
		std::cout << "Failed to open " << filepath << std::endl;
		return -1;
	}
	video.set(cv::CAP_PROP_POS_FRAMES, 2060);

	int altimeter_mask_offset = 0;
	int airspeed_mask_offset = 0;
	cv::Mat frame;
	AltimeterTracker altimeter_tracker;
	AirspeedIndicatorTracker airspeed_tracker;

	while (1)
	{
		// load frame
		video >> frame;
		if (frame.empty())
			break;

		altimeter_tracker.set_initial_mask_offset(altimeter_mask_offset);
		altimeter_tracker.set_input_image(frame);
		altimeter_tracker.fit();
		altimeter_mask_offset = altimeter_tracker.get_mask_offset();
		std::vector<cv::Rect> altimeter_mask_pos = altimeter_tracker.get_numbers_bbox();

		airspeed_tracker.set_initial_mask_offset(airspeed_mask_offset);
		airspeed_tracker.set_input_image(frame);
		airspeed_tracker.fit();
		airspeed_mask_offset = airspeed_tracker.get_mask_offset();
		std::vector<cv::Rect> airspeed_mask_pos = airspeed_tracker.get_numbers_bbox();

		for (auto itr = altimeter_mask_pos.begin(); itr != altimeter_mask_pos.end(); ++itr)
		{
			cv::rectangle(frame, *itr, cv::Scalar(0, 255, 0), 1);
		}

		for (auto itr = airspeed_mask_pos.begin(); itr != airspeed_mask_pos.end(); ++itr)
		{
			cv::rectangle(frame, *itr, cv::Scalar(0, 255, 0), 1);
		}

		cv::imshow("frame", frame);
		cv::waitKey(1);

	}
}
