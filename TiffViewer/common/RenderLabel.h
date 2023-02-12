#pragma once
#include <opencv2/opencv.hpp>

#include <QWidget>
#include "ui_RenderLabel.h"

class RenderLabel : public QWidget
{
	Q_OBJECT

public:
	RenderLabel(QWidget *parent = nullptr);
	~RenderLabel();

private:
	Ui::RenderLabelClass ui;
private:
	QTimer* timer = nullptr;
private:
	cv::Mat render_mat;
	cv::Mat origin_mat;
	cv::Rect render_roi;
	//----- 视图变换参数
	// 缩放比例
	float view_scale_last = 1.0f;
	float view_scale = 1.0f;
	// 偏移
	QPoint view_move;
	// 缩放中心
	QPoint view_center;
	//----- 图像调整参数
	// 亮度
	int param_brightness = 0;
	// 对比度 是1.0*100倍
	int param_contrast = 100;
	// 是否反色
	bool param_is_inverse_color = false;
	//---- 重绘控制
	bool is_update_brigheness = false;
	bool is_update_contrast = false;
	bool is_update_inverse_color = false;
	bool is_update_transformed = false;
private:
	QPoint move_center;
	QPoint move_press_value;
	QPoint move_value;
	bool move_left_is_clicked = false;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
private:
	/// <summary>
	/// 自适应视图，调整图片大小到视图并居中显示
	/// </summary>
	void adaptive_view();
private:
	/// <summary>
	/// 变换视图
	/// </summary>
	void transform(float scale, cv::Point offset, cv::Point center = cv::Point(0, 0));
	void brightness(const int value = 0);
	void contrast(const int value = 1.0);
	void inverse_color(bool is_inverse = false);
	void zoom_in();
	void zoom_out();
	void zoom_reset();
public slots:
	/// <summary>
	/// 更新视图
	/// </summary>
	void update_view();
	/// <summary>
	/// 设置新的图片并自适应缩放以及居中显示
	/// </summary>
	void set_render_mat(const cv::Mat& mat);
	void set_brightness(const int value) { brightness(value); }
	void set_contrast(const int value) { contrast(value); }
	void set_inverse_color(bool is_inverse) { inverse_color(is_inverse); }
	void set_zoom_in() { zoom_in(); }
	void set_zoom_out() { zoom_out(); }
	void set_zoom_reset() { zoom_reset(); }
	void set_adaptive_view() { adaptive_view(); }
signals:
	void signal_open_file();
};	
