#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include <array>
#include <numeric>
#include <windows.h>

#include "AltimeterTracker.h"
#include "AirspeedIndicatorTracker.h"
#include "CompassTracker.h"


inline float offset_to_altitude(int offset)
{
	return 0.0237 * offset + 25;
}

inline float offset_to_airspeed(int offset)
{
	return 0.234 * offset + 39.9;
}

inline float offset_to_direction(int offset)
{
	return - 0.0346 * offset + 31.7;
}

int main()
{
	/*
	 * scene:
	 * frame 0 -- frame 2007: Ground 
	 * frame 2008 -- frame 2015: noise
	 * frame 2016 -- frame 2754: Ingressing
	 * frame 2755 -- frame 2770: noise
	 * frame 2771 -- frame 4276: Ingressing (detecting a spike on right)
	 * frame 4277 -- frame 16060: dropping bomb and dodging
	 * frame 16061 -- frame 17587: dog fight mode
	 * frame 17588 -- frame 17940: end
	 */

	/*
	 * position of compass on the HUD
	 * frame 0 -- frame 4276 lower space
	 * frame 4277 -- frame 16060 upper space
	 * frame 16061 -- frame 17587 not shown on HUD
	 * frame 17588 -- frame 17940 not fixed
	k*/

	// open mp4
	std::string filepath = "..\\movie.mp4";
	cv::VideoCapture video;
	video.open(filepath);
	
	int altimeter_mask_offset = 0;
	int airspeed_mask_offset = 0;
	int compass_mask_offset = 0;

	float altimeter_diff = 0.f;
	float airspeed_diff = 0.f;
	float compass_diff = 0.f;

	cv::Mat frame;
	long frame_num = 2000;
	AltimeterTracker altimeter_tracker;
	AirspeedIndicatorTracker airspeed_tracker;
	CompassTracker compass_tracker;

	std::ofstream file;

	if (!video.isOpened())
	{
		std::cout << "Failed to open " << filepath << std::endl;
		return -1;
	}
	video.set(cv::CAP_PROP_POS_FRAMES, frame_num);
	file.open("diff.csv");
	file << "frame,altitude_diff,airspeed_diff,direction_diff" << std::endl;

	while (1)
	{
		std::cout << frame_num << " " << std::endl;
		// load frame
		video >> frame;
		if (frame.empty())
			break;

		altimeter_tracker.set_initial_mask_offset(altimeter_mask_offset);
		altimeter_tracker.set_input_image(frame);
		altimeter_tracker.fit();
		altimeter_diff = altimeter_tracker.get_altitude_diff();
		altimeter_mask_offset = altimeter_tracker.get_mask_offset();
		std::vector<cv::Rect> altimeter_mask_pos = altimeter_tracker.get_numbers_bbox();

		airspeed_tracker.set_initial_mask_offset(airspeed_mask_offset);
		airspeed_tracker.set_input_image(frame);
		airspeed_tracker.fit();
		airspeed_diff = airspeed_tracker.get_airspeed_diff();
		airspeed_mask_offset = airspeed_tracker.get_mask_offset();
		std::vector<cv::Rect> airspeed_mask_pos = airspeed_tracker.get_numbers_bbox();

		compass_tracker.set_initial_mask_offset(compass_mask_offset);
		compass_tracker.set_input_image(frame);
		if (frame_num <= 4277)
			compass_tracker.set_compass_pos_upper(false);
		else if (frame_num >= 9059 && frame_num <= 9101)
			compass_tracker.set_compass_pos_upper(false);
		else
			compass_tracker.set_compass_pos_upper(true);
		compass_tracker.fit();
		compass_diff = compass_tracker.get_compass_diff();
		compass_mask_offset = compass_tracker.get_mask_offset();
		std::vector<cv::Rect> compass_mask_pos = compass_tracker.get_numbers_bbox();

		// for (auto itr = altimeter_mask_pos.begin(); itr != altimeter_mask_pos.end(); ++itr)
		// {
		// 	cv::rectangle(frame, *itr, cv::Scalar(0, 255, 0), 1);
		// }

		// for (auto itr = airspeed_mask_pos.begin(); itr != airspeed_mask_pos.end(); ++itr)
		// {
		// 	cv::rectangle(frame, *itr, cv::Scalar(0, 255, 0), 1);
		// }

		// for (auto itr = compass_mask_pos.begin(); itr != compass_mask_pos.end(); ++itr)
		// {
		// 	cv::rectangle(frame, *itr, cv::Scalar(0, 255, 0), 1);
		// }

		// file << offset_to_altitude(altimeter_mask_offset) << ","
		// 	<< offset_to_airspeed(airspeed_mask_offset) << ","
		// 	<< offset_to_direction(compass_mask_offset) << "\n";
		file << frame_num << "," << altimeter_diff << "," << airspeed_diff << "," << compass_diff << std::endl;

		// cv::imshow("frame", frame);
		// cv::waitKey(1);
		frame_num++;
	}
	file.close();
	return 0;
}
