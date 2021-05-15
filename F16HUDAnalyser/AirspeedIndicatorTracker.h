#pragma once

#include <opencv2/opencv.hpp>

class AirspeedIndicatorTracker
{
public:
	void fit();
	std::vector<cv::Rect> get_numbers_bbox();

	inline void set_input_image(cv::Mat image)
	{
		frame = image;
	}

	inline void set_initial_mask_offset(int offset)
	{
		mask_offset = offset;
	}

	inline int get_mask_offset()
	{
		return mask_offset;
	}

private:
	static const int NUM_OF_MASKS = 4;
	static const int MASK_WIDTH = 12;
	static const int MASK_HEIGHT = 9;
	static const int MASK_SLIDE_MIN = -10;
	static const int MASK_SLIDE_MAX = 10;
	static const int MASK_INTERVAL = 21;
	static const int INDICATOR_POS_X = 62;
	static const int INDICATOR_POS_Y = 67;
	static const int INDICATOR_NUM_POS_X = 59;
	static const int INDICATOR_NUM_POS_Y = 59;
	static const int INDICATOR_WIDTH = 32;
	static const int INDICATOR_HEIGHT = 88;
	static const int INDICATOR_NUM_WIDTH = 15;
	static const int INDICATOR_NUM_HEIGHT = 83;

	cv::Mat frame, frame_gray, mask_canvas_gray, bitwise_and_dst;
	cv::Rect mask;
	int mask_offset;
	int best_mask_slide = 0;
	int max_count_nonzero = 0;
	std::array<int, NUM_OF_MASKS> count_nonzero;
	std::array<int, NUM_OF_MASKS> best_count_nonzero = { 0, 0, 0, 0 };

	void clear_parameters();
};
