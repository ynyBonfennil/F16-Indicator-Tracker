#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <array>
#include <numeric>
#include <windows.h>


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

	cv::Mat image;
	cv::Mat altimeter, altimeter_numbers, altimeter_numbers_gray;
	int frame = 0;

	cv::Rect rect_altimeter = cv::Rect(cv::Point(254, 59), cv::Point(286, 147));
	cv::Rect rect_altimeter_numbers = cv::Rect(cv::Point(263, 59), cv::Point(286, 147));
	const int MASK_WIDTH = 23;
	const int MASK_HEIGHT = 12;
	const int MASK_SLIDE_MIN = -10;
	const int MASK_SLIDE_MAX = 10;
	const int MASK_INTERVAL = 21;
	int mask_offset = 0;

	// analyse frame by frame
	while (1)
	{
		// load frame
		video >> image;
		image(rect_altimeter_numbers).copyTo(altimeter_numbers);
		if (image.empty())
			break;
		cv::cvtColor(altimeter_numbers, altimeter_numbers_gray, cv::COLOR_BGR2GRAY);
		cv::Canny(altimeter_numbers_gray, altimeter_numbers_gray, 100, 200);

		// create masks of altimeter (especially indicator numbers)
		// numbers detection is realized by sliding masks and
		// calculating bitwise and Canny edge detection result and the mask.
		// for each frame
		int best_mask_slide = 0;
		std::array<int, 5> best_count_nonzero = { 0, 0, 0, 0, 0 };
		int max_count_nonzero = 0;
		for (int mask_slide = MASK_SLIDE_MIN; mask_slide < MASK_SLIDE_MAX; mask_slide++)
		{
			cv::Mat mask_canvas, mask_canvas_gray, bitwise_and_dst;
			cv::Rect mask;
			std::array<int, 5> count_nonzero;

			for (int mask_num = 0; mask_num < 5; mask_num++)
			{
				mask_canvas = cv::Mat(88, 23, CV_8UC3, cv::Scalar(0));
				mask = cv::Rect(0, mask_offset%MASK_INTERVAL + mask_slide + MASK_INTERVAL * mask_num, MASK_WIDTH, MASK_HEIGHT);
				cv::rectangle(mask_canvas, mask, cv::Scalar(255, 255, 255), -1);
				cv::cvtColor(mask_canvas, mask_canvas_gray, cv::COLOR_BGR2GRAY);
				cv::bitwise_and(altimeter_numbers_gray, mask_canvas_gray, bitwise_and_dst);
				count_nonzero[mask_num] = cv::countNonZero(bitwise_and_dst);
			}

			int count_nonzero_sum = std::accumulate(count_nonzero.begin(), count_nonzero.end(), 0);
			if (count_nonzero_sum > max_count_nonzero)
			{
				max_count_nonzero = count_nonzero_sum;
				best_count_nonzero = count_nonzero;
				best_mask_slide = mask_slide;
			}
			
		}
		mask_offset += best_mask_slide;
		std::cout << mask_offset << " " << best_mask_slide << std::endl;

		// draw rectangles on the numbers detected
		for (int mask_num = 0; mask_num < 5; mask_num++)
		{
			if (best_count_nonzero[mask_num] > 25)
			{
				cv::Rect rect(0, mask_offset%MASK_INTERVAL + MASK_INTERVAL * mask_num, MASK_WIDTH, MASK_HEIGHT);
				cv::rectangle(altimeter_numbers, rect, cv::Scalar(0, 255, 0), 1);
			}
		}
		cv::imshow("mask", altimeter_numbers);
		cv::waitKey(1);
		frame++;
	}
}
