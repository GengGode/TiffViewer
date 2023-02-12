#pragma once

#include <QImage>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace ConvertUtils
{
	struct view_param
	{
		// 缩放比例
		float scale = 1.0f;
		// 偏移
		cv::Point offset = cv::Point(0, 0);
	};

	cv::Mat qimage_to_mat(const QImage& qimage);
	
	QImage mat_to_qimage(const cv::Mat& mat);
	
	cv::Mat tiff_to_8bit(const cv::Mat& tiff_mat);
	
	std::pair<std::vector<std::string>, int> get_tiff_file_vector(const std::string& tiff_file);

	/// <summary>
	/// 计算变换参数
	/// </summary>
	/// <param name="view_size">视图区域大小</param>
	/// <param name="mat_size">图片区域大小</param>
	/// <returns></returns>
	view_param calc_transform_adaptive(QSize view_size, cv::Size mat_size);

	void draw_transparent_rounded_rect(cv::Mat& mat, int radius);

	void brightness_and_contrast(cv::Mat& mat, float brightness, float contrast);
	
	void inverse_color(cv::Mat& mat, bool is_lock_alpha = false);
}
