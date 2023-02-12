#include "ConvertUtils.h"

QImage ConvertUtils::mat_to_qimage(cv::Mat const& mat)
{
	if (mat.channels() == 1)
	{
		//QImage image = QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, QImage::Format_Indexed8);
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		uchar* pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar* pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	else if (mat.channels() == 3)
	{
		QImage image = QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, QImage::Format_BGR888);
		image.bits(); // enforce deep copy, see documentation 
		return image;
	}
	if (mat.channels() == 4)
	{
		QImage dest((const uchar*)(mat.data), mat.cols, mat.rows, mat.cols * (mat.channels()), QImage::Format_ARGB32);
		dest.bits(); // enforce deep copy, see documentation 
		return dest;
	}
	else
	{
		return QImage(100, 100, QImage::Format_BGR888);
	}
}

cv::Mat ConvertUtils::qimage_to_mat(QImage const& src)
{
	cv::Mat tmp(src.height(), src.width(), CV_8UC4, (uchar*)src.bits(), src.bytesPerLine());
	cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
	result = tmp.clone();
	return result;
}

cv::Mat ConvertUtils::tiff_to_8bit(const cv::Mat& tiff_mat)
{
	if (tiff_mat.empty())
	{
		return cv::Mat();
	}
	cv::Mat tiff_8bit_mat;
	if (tiff_mat.channels() == 1)
	{
		tiff_mat.convertTo(tiff_8bit_mat, CV_8U);
	}
	else if (tiff_mat.channels() == 3)
	{
		tiff_mat.convertTo(tiff_8bit_mat, CV_8UC3);
	}
	else if (tiff_mat.channels() == 4)
	{
		tiff_mat.convertTo(tiff_8bit_mat, CV_8UC4);
	}
	return tiff_8bit_mat;
}

std::pair<std::vector<std::string>, int> ConvertUtils::get_tiff_file_vector(const std::string& tiff_file)
{
	auto file = std::filesystem::path(tiff_file);
	auto dir = file.parent_path();
	std::vector<std::string> tiff_file_vector;
	int tiff_file_vector_it = -1;
	for (auto& p : std::filesystem::directory_iterator(dir))
	{
		auto& path = p.path();
		if (path.extension() == ".tif" || path.extension() == ".tiff")
		{
			tiff_file_vector.push_back(path.string());
			if (path.filename() == file.filename())
			{
				tiff_file_vector_it = tiff_file_vector.size() - 1;
			}
		}
	}
	return { tiff_file_vector ,tiff_file_vector_it };
}

ConvertUtils::view_param ConvertUtils::calc_transform_adaptive(QSize view_size, cv::Size mat_size)
{
	view_param param;
	// 计算缩放比例
	float scale_x = (float)view_size.width() / (float)mat_size.width;
	float scale_y = (float)view_size.height() / (float)mat_size.height;
	param.scale = scale_x < scale_y ? scale_x : scale_y;
	if (param.scale > 2.0f)
	{
		param.scale = 2.0f;
	}
	// 计算偏移
	param.offset.x = static_cast<int>((view_size.width() - mat_size.width * param.scale) / 2.0);
	param.offset.y = static_cast<int>((view_size.height() - mat_size.height * param.scale) / 2.0);
	if (param.offset.x < 0)
	{
		param.offset.x = 0;
	}
	if (param.offset.y < 0)
	{
		param.offset.y = 0;
	}
	return param;
}

void ConvertUtils::draw_transparent_rounded_rect(cv::Mat& mat, int radius)
{
	if (mat.empty())
	{
		return;
	}
	if (mat.channels() != 4)
	{
		return;
	}
	if (mat.cols < radius * 2 || mat.rows < radius * 2)
	{
		return;
	}
	cv::Mat mask(radius * 2, radius * 2, CV_8UC4, cv::Scalar(0));
	mat(cv::Rect(0, 0, radius, radius)).copyTo(mask(cv::Rect(0, 0, radius, radius)));
	mat(cv::Rect(mat.cols - radius, 0, radius, radius)).copyTo(mask(cv::Rect(radius, 0, radius, radius)));
	mat(cv::Rect(0, mat.rows - radius, radius, radius)).copyTo(mask(cv::Rect(0, radius, radius, radius)));
	mat(cv::Rect(mat.cols - radius, mat.rows - radius, radius, radius)).copyTo(mask(cv::Rect(radius, radius, radius, radius)));
	
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			int x = j - radius;
			int y = i - radius;
			if (x * x + y * y > radius * radius)
			{
				mask.at<cv::Vec4b>(i, j) = cv::Vec4b(0, 0, 0, 0);
			}
		}
	}
	cv::Mat roi_lt = mask(cv::Rect(0, 0, radius, radius));
	cv::Mat roi_rt = mask(cv::Rect(radius, 0, radius, radius));
	cv::Mat roi_lb = mask(cv::Rect(0, radius, radius, radius));
	cv::Mat roi_rb = mask(cv::Rect(radius, radius, radius, radius));
	
	roi_lt.copyTo(mat(cv::Rect(0, 0, radius, radius)));
	roi_rt.copyTo(mat(cv::Rect(mat.cols - radius, 0, radius, radius)));
	roi_lb.copyTo(mat(cv::Rect(0, mat.rows - radius, radius, radius)));
	roi_rb.copyTo(mat(cv::Rect(mat.cols - radius, mat.rows - radius, radius, radius)));
}

void ConvertUtils::brightness_and_contrast(cv::Mat& mat, float brightness, float contrast)
{
	mat.convertTo(mat, -1, contrast, brightness);
	
}

void ConvertUtils::inverse_color(cv::Mat& mat, bool is_lock_alpha )
{
	if (mat.channels() == 1)
	{
		cv::Mat tmp;
		cv::bitwise_not(mat, tmp);
		mat = tmp.clone();
	}
	else if (mat.channels() == 3)
	{
		cv::Mat tmp;
		cv::bitwise_not(mat, tmp);
		mat = tmp.clone();
	}
	else if (mat.channels() == 4)
	{
		cv::Mat tmp;
		cv::bitwise_not(mat, tmp);
		if (is_lock_alpha)
		{
			for (int i = 0; i < mat.rows; i++)
			{
				for (int j = 0; j < mat.cols; j++)
				{
					tmp.at<cv::Vec4b>(i, j)[3] = mat.at<cv::Vec4b>(i, j)[3];
				}
			}
		}
		mat = tmp.clone();
	}
}