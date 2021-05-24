#pragma once

#include <opencv2/opencv.hpp>

class AltimeterTracker
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

	inline float get_altitude_diff()
	{
		return ((float)best_mask_slide / MASK_INTERVAL) * 0.5;
	}

	inline int get_mask_offset()
	{
		return mask_offset;
	}

private:
	static const int NUM_OF_MASKS = 5;
	static const int MASK_WIDTH = 23;
	static const int MASK_HEIGHT = 9;
	static const int MASK_SLIDE_MIN = -10;
	static const int MASK_SLIDE_MAX = 10;
	static const int MASK_INTERVAL = 21;
	static const int ALTIMETER_POS_X = 254;
	static const int ALTIMETER_POS_Y = 59;
	static const int ALTIMETER_NUM_POS_X = 263;
	static const int ALTIMETER_NUM_POS_Y = 59;
	static const int ALTIMETER_WIDTH = 32;
	static const int ALTIMETER_HEIGHT = 88;
	static const int ALTIMETER_NUM_WIDTH = 23;
	static const int ALTIMETER_NUM_HEIGHT = 88;

	cv::Mat frame, frame_gray, mask_canvas_gray, bitwise_and_dst;
	cv::Rect mask;
	int mask_offset;
	int best_mask_slide = 0;
	int max_count_nonzero = 0;
	std::array<int, NUM_OF_MASKS> count_nonzero;
	std::array<int, NUM_OF_MASKS> best_count_nonzero = { 0, 0, 0, 0 };

	void clear_parameters();
	void update_count_nonzero(int mask_slide);
};
