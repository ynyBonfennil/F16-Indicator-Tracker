#include <numeric>
#include "CompassTracker.h"

void CompassTracker::fit()
{
	clear_parameters();
	cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	cv::Canny(frame_gray, frame_gray, 100, 200);

	int compass_num_pos_y = 0;
	if (is_upper)
		compass_num_pos_y = COMPASS_NUM_POS_Y_UPPER;
	else
		compass_num_pos_y = COMPASS_NUM_POS_Y_LOWER;

	// slide minus
	for (int mask_slide = 0; mask_slide >= MASK_SLIDE_MIN; mask_slide--)
	{
		update_count_nonzero(compass_num_pos_y, mask_slide);
		int count_nonzero_sum = std::accumulate(count_nonzero.begin(), count_nonzero.end(), 0);

		if (count_nonzero_sum < max_count_nonzero)
			break;

		max_count_nonzero = count_nonzero_sum;
		best_count_nonzero = count_nonzero;
		best_mask_slide = mask_slide;
	}

	// slide plus
	for (int mask_slide = 1; mask_slide < MASK_SLIDE_MAX; mask_slide++)
	{
		update_count_nonzero(compass_num_pos_y, mask_slide);
		int count_nonzero_sum = std::accumulate(count_nonzero.begin(), count_nonzero.end(), 0);

		if (count_nonzero_sum < max_count_nonzero)
			break;
		max_count_nonzero = count_nonzero_sum;
		best_count_nonzero = count_nonzero;
		best_mask_slide = mask_slide;
	}

	mask_offset += best_mask_slide;
}

std::vector<cv::Rect> CompassTracker::get_numbers_bbox()
{
	std::vector<cv::Rect> mask_pos = {};

	int compass_num_pos_y = 0;
	if (is_upper)
		compass_num_pos_y = COMPASS_NUM_POS_Y_UPPER;
	else
		compass_num_pos_y = COMPASS_NUM_POS_Y_LOWER;

	// draw rectangles on the numbers detected
	for (int mask_num = 0; mask_num < NUM_OF_MASKS; mask_num++)
	{
		if (best_count_nonzero[mask_num] > -1)
		{
			cv::Rect rect(
				COMPASS_NUM_POS_X + mask_offset % MASK_INTERVAL + MASK_INTERVAL * mask_num,
				compass_num_pos_y,
				MASK_WIDTH,
				MASK_HEIGHT);
			mask_pos.push_back(rect);
		}
	}
	return mask_pos;
}

void CompassTracker::clear_parameters()
{
	best_mask_slide = 0;
	max_count_nonzero = 0;
	count_nonzero = {};
	best_count_nonzero = {};
}

void CompassTracker::update_count_nonzero(int compass_num_pos_y, int mask_slide)
{
	for (int mask_num = 0; mask_num < NUM_OF_MASKS; mask_num++)
	{
		mask_canvas_gray = cv::Mat(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
		mask = cv::Rect(
			COMPASS_NUM_POS_X + mask_offset % MASK_INTERVAL + MASK_INTERVAL * mask_num + mask_slide,
			compass_num_pos_y,
			MASK_WIDTH,
			MASK_HEIGHT);
		cv::rectangle(mask_canvas_gray, mask, cv::Scalar(255, 255, 255), -1);
		cv::bitwise_and(frame_gray, mask_canvas_gray, bitwise_and_dst);
		count_nonzero[mask_num] = cv::countNonZero(bitwise_and_dst);
	}
}