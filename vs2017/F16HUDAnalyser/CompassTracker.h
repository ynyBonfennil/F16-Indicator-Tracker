#pragma once

#include <opencv2/opencv.hpp>

class CompassTracker
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

	inline float get_compass_diff()
	{
		return -1 * (float)best_mask_slide / MASK_INTERVAL;
	}

	inline void set_compass_pos_upper(bool is_upper)
	{
		this->is_upper = is_upper;
	}

private:
	static const int NUM_OF_MASKS = 3;
	static const int MASK_WIDTH = 14;
	static const int MASK_HEIGHT = 9;
	static const int MASK_SLIDE_MIN = -10;
	static const int MASK_SLIDE_MAX = 10;
	static const int MASK_INTERVAL = 34;
	static const int COMPASS_POS_X = 254;
	static const int COMPASS_POS_Y = 59;
	static const int COMPASS_NUM_POS_X = 120;
	static const int COMPASS_NUM_POS_Y_UPPER = 30;
	static const int COMPASS_NUM_POS_Y_LOWER = 151;
	static const int COMPASS_WIDTH = 95;
	static const int COMPASS_HEIGHT = 17;
	static const int COMPASS_NUM_WIDTH = 95;
	static const int COMPASS_NUM_HEIGHT = 17;

	cv::Mat frame, frame_gray, mask_canvas_gray, bitwise_and_dst;
	cv::Rect mask;
	int mask_offset;
	int best_mask_slide = 0;
	int max_count_nonzero = 0;
	std::array<int, NUM_OF_MASKS> count_nonzero;
	std::array<int, NUM_OF_MASKS> best_count_nonzero = {};
	bool is_upper;

	void clear_parameters();
	void update_count_nonzero(int compass_num_pos_y, int mask_slide);
};

