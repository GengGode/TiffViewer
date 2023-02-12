#include "RenderLabel.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

// utils
#include "../ConvertUtils.h"

RenderLabel::RenderLabel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// ��ʼ����ʱ��
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &RenderLabel::update_view);
	timer->start(33);
}

RenderLabel::~RenderLabel()
{}

void RenderLabel::set_render_mat(const cv::Mat& mat)
{
	if (mat.empty())
	{
		return;
	}
	//---- ���ݸ�λ
	move_value = QPoint(0, 0);
	//-----------
	origin_mat = mat.clone();
	adaptive_view();
}


void RenderLabel::update_view()
{
	if (origin_mat.empty())
	{
		return;
	}
	if (is_update_transformed == false && 
		is_update_brigheness == false &&
		is_update_contrast == false && 
		is_update_inverse_color == false)
	{
		return;
	}
	transform(view_scale, cv::Point(view_move.x(), view_move.y()), cv::Point(move_center.x(), move_center.y()));
	cv::Mat roi = render_mat(render_roi);
	// Brightness or Contrast
	if (is_update_transformed||is_update_brigheness || is_update_contrast)
	{
		ConvertUtils::brightness_and_contrast(roi, param_brightness, param_contrast/100.0);
		is_update_brigheness = false;
		is_update_contrast = false;
	}
	// Inverse Color
	if (is_update_transformed|| is_update_inverse_color)
	{
		if (param_is_inverse_color)
		{
			bool is_lock_alpha = false;
			if (origin_mat.channels() != 4)
			{
				is_lock_alpha = true;
			}
			ConvertUtils::inverse_color(roi, is_lock_alpha);
			is_update_inverse_color = false;
		}
	}
	roi.copyTo(render_mat(render_roi));
	// Update
	update();
}

void RenderLabel::transform(float scale, cv::Point offset, cv::Point center)
{
	render_mat = cv::Mat(width(), height(), CV_8UC4, cv::Scalar(128, 128, 128, 0));
	// ����任
	cv::Mat affine_mat = cv::getRotationMatrix2D(center - offset , 0, scale);
	affine_mat.at<double>(0, 2) += offset.x ;
	affine_mat.at<double>(1, 2) += offset.y ;
	cv::warpAffine(origin_mat, render_mat, affine_mat, cv::Size(width(), height()));
	
	
	qDebug() << "move_value: [" << move_value << "]";
	qDebug() << "move_center: [" << move_center << "]";
	qDebug() << "move_press_value: [" << move_press_value << "]";
	qDebug() << "offset: [" << offset.x << "," << offset.y << "]";
	qDebug() << "center: [" << center.x << "," << center.y << "]";
	qDebug() << "scale: " << scale << "\n";

	

	
	// ����ͼ����Size
	cv::Size viewer__size = cv::Size(width(), height());
	// ����ͼ����Rect
	cv::Rect viewer__rect = cv::Rect(0, 0, width(), height());
	// ���ɡ���ͼ�� 4ͨ��
	//render_mat = cv::Mat(viewer__size, CV_8UC4, cv::Scalar(128, 128, 128, 0));
	
	//--- ���㲿��
	// ���㡾�任��ͼƬ����С
	cv::Size transformed_mat__size = cv::Size(cvRound(origin_mat.cols * scale), cvRound(origin_mat.rows * scale));
	auto p = offset - center;
	
	// ���㡾�任��ͼƬ����Rect
	cv::Rect transformed_mat__rect = cv::Rect(
		offset.x + (move_center.x()- offset.x)*(view_scale - view_scale_last),
		offset.y + (move_center.y() - offset.y) * (view_scale - view_scale_last),
		transformed_mat__size.width, transformed_mat__size.height);
	
	// ���㡾�任��ͼƬ���롾��ͼ���ġ��ཻ����
	cv::Rect intersect__rect = transformed_mat__rect & viewer__rect;
	
	// ���㡾�ཻ�����ڡ��任��ͼƬ���е�Rect
	cv::Rect intersect__rect_in_transformed_mat__rect = cv::Rect(intersect__rect.x - transformed_mat__rect.x, intersect__rect.y - transformed_mat__rect.y, intersect__rect.width, intersect__rect.height);
	// ���㡾�ཻ�����ڡ���ͼ���е�Rect
	cv::Rect intersect__rect_in_viewer__rect = cv::Rect(intersect__rect.x - viewer__rect.x, intersect__rect.y - viewer__rect.y, intersect__rect.width, intersect__rect.height);
	// ���㡾�ཻ�����ڱ任ǰ��ԭͼ���е�Rect
	cv::Rect intersect__rect_in_origin_mat__rect = cv::Rect(cvRound((intersect__rect.x - transformed_mat__rect.x) / scale), cvRound((intersect__rect.y - transformed_mat__rect.y) / scale), cvRound(intersect__rect.width / scale), cvRound(intersect__rect.height / scale));
	// Խ�紦��
	if (intersect__rect_in_origin_mat__rect.x < 0)
	{
		intersect__rect_in_origin_mat__rect.x = 0;
	}
	if (intersect__rect_in_origin_mat__rect.y < 0)
	{
		intersect__rect_in_origin_mat__rect.y = 0;
	}
	if (intersect__rect_in_origin_mat__rect.x + intersect__rect_in_origin_mat__rect.width > origin_mat.cols)
	{
		intersect__rect_in_origin_mat__rect.width = origin_mat.cols - intersect__rect_in_origin_mat__rect.x;
	}
	if (intersect__rect_in_origin_mat__rect.y + intersect__rect_in_origin_mat__rect.height > origin_mat.rows)
	{
		intersect__rect_in_origin_mat__rect.height = origin_mat.rows - intersect__rect_in_origin_mat__rect.y;
	}
	
	// û���ཻ���� ����ȫ͸��
	if(intersect__rect.area() == 0)
	{
		return;
	}
	
	//--- ͼƬ��������
	// ȡ�����ཻ�����ڡ�ԭͼ���еĲ���
	cv::Mat origin_mat__intersect = origin_mat(intersect__rect_in_origin_mat__rect);
	// ���š��ཻ����
	cv::Mat origin_mat__intersect__transformed;
	cv::resize(origin_mat__intersect, origin_mat__intersect__transformed, intersect__rect_in_transformed_mat__rect.size());
	// ����ǰ���ͨ������
	if (origin_mat__intersect__transformed.channels() == 1)
	{
		cv::cvtColor(origin_mat__intersect__transformed, origin_mat__intersect__transformed, cv::COLOR_GRAY2BGRA);
	}
	else if (origin_mat__intersect__transformed.channels() == 3)
	{
		cv::cvtColor(origin_mat__intersect__transformed, origin_mat__intersect__transformed, cv::COLOR_BGR2BGRA);
	}
	// �����ཻ���򡿸��Ƶ�����ͼ����
	//origin_mat__intersect__transformed.copyTo(render_mat(intersect__rect_in_viewer__rect));
	render_roi = intersect__rect_in_viewer__rect;
}

void RenderLabel::adaptive_view()
{
	auto param = ConvertUtils::calc_transform_adaptive(this->size(), origin_mat.size());
	view_scale = param.scale;
	view_move = QPoint(param.offset.x, param.offset.y);
	move_value = view_move;
	transform(param.scale, param.offset);
}

void RenderLabel::brightness(const int value)
{	
	if (param_brightness != value)
	{
		param_brightness = value;
		is_update_brigheness = true;
	}
}

void RenderLabel::contrast(const int value)
{
	if (param_contrast != value)
	{
		param_contrast = value;
		is_update_contrast = true;
	}
}

void RenderLabel::inverse_color(bool is_inverse)
{
	if (param_is_inverse_color != is_inverse)
	{
		param_is_inverse_color = is_inverse;
		is_update_inverse_color = true;
	}
}

void RenderLabel::zoom_in()
{
	// Zoom In
	view_scale_last = view_scale;
	view_scale *= 1.1f;
	if (view_scale > 200.0f)
		view_scale = 200.0f;
}

void RenderLabel::zoom_out()
{
	// Zoom Out
	view_scale_last = view_scale;
	view_scale /= 1.1f;
	if (view_scale < 0.001f)
		view_scale = 0.001f;
}

void RenderLabel::zoom_reset()
{
	// Zoom Reset
	view_scale = 1.0f;
}

#pragma region �����ػ��¼�
void RenderLabel::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		move_left_is_clicked = true;
		move_press_value = event->globalPos() - this->pos();
		event->accept();
	}
}

void RenderLabel::mouseMoveEvent(QMouseEvent* event)
{
	if (move_left_is_clicked)
	{
		view_move = move_value + (event->globalPos() - move_press_value) / 1.0;// view_scale;
		is_update_transformed = true;
		event->accept();
	}
}

void RenderLabel::mouseReleaseEvent(QMouseEvent* event)
{
	move_left_is_clicked = false;
	move_value = view_move;
}

void RenderLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (render_mat.empty())
		{
			emit signal_open_file();
			return;
		}
		is_update_transformed = true;
		event->accept();
	}
}


void RenderLabel::wheelEvent(QWheelEvent* event)
{
	if (event->delta() > 0)
	{
		zoom_in();
	}
	else
	{
		zoom_out();
	}
	move_center = event->pos();
	qDebug() << move_center;
	is_update_transformed = true;
}

void RenderLabel::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	if (render_mat.empty())
	{
		painter.fillRect(this->rect(), Qt::transparent);
		return;
	}
	// �˴�����Ⱦ����ͼƬת��ΪQPixmap��ֱ����ʾ�����е�ǰ������render_mat�����
	ConvertUtils::draw_transparent_rounded_rect(render_mat, 8);
	QPixmap render_pixmap = QPixmap::fromImage(ConvertUtils::mat_to_qimage(render_mat));
	painter.drawPixmap(0, 0, render_pixmap);

}
#pragma endregion

