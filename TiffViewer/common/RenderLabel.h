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
	//----- ��ͼ�任����
	// ���ű���
	float view_scale_last = 1.0f;
	float view_scale = 1.0f;
	// ƫ��
	QPoint view_move;
	// ��������
	QPoint view_center;
	//----- ͼ���������
	// ����
	int param_brightness = 0;
	// �Աȶ� ��1.0*100��
	int param_contrast = 100;
	// �Ƿ�ɫ
	bool param_is_inverse_color = false;
	//---- �ػ����
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
	/// ����Ӧ��ͼ������ͼƬ��С����ͼ��������ʾ
	/// </summary>
	void adaptive_view();
private:
	/// <summary>
	/// �任��ͼ
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
	/// ������ͼ
	/// </summary>
	void update_view();
	/// <summary>
	/// �����µ�ͼƬ������Ӧ�����Լ�������ʾ
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
