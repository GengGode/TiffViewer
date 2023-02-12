#pragma once

#include <QImage>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace ConvertUtils
{
	struct view_param
	{
		// ���ű���
		float scale = 1.0f;
		// ƫ��
		cv::Point offset = cv::Point(0, 0);
	};

	cv::Mat qimage_to_mat(const QImage& qimage);
	
	QImage mat_to_qimage(const cv::Mat& mat);
	
	cv::Mat tiff_to_8bit(const cv::Mat& tiff_mat);
	
	std::pair<std::vector<std::string>, int> get_tiff_file_vector(const std::string& tiff_file);

	/// <summary>
	/// ����任����
	/// </summary>
	/// <param name="view_size">��ͼ�����С</param>
	/// <param name="mat_size">ͼƬ�����С</param>
	/// <returns></returns>
	view_param calc_transform_adaptive(QSize view_size, cv::Size mat_size);

	void draw_transparent_rounded_rect(cv::Mat& mat, int radius);

	void brightness_and_contrast(cv::Mat& mat, float brightness, float contrast);
	
	void inverse_color(cv::Mat& mat, bool is_lock_alpha = false);
}
