#pragma once

#include <QtWidgets/QWidget>
#include "ui_TiffViewer.h"

#include "common/RenderLabel.h"
#include "common/BorderManage.h"


class TiffViewer : public QWidget
{
    Q_OBJECT

public:
    TiffViewer(QWidget *parent = nullptr);
    ~TiffViewer();

private:
    Ui::TiffViewerClass ui;
private:
    RenderLabel* render_label = nullptr;
	BorderManage* border_manage = nullptr;
private:
    bool is_endless_loop = false;
private:
	std::vector<std::string> tiff_file_vector;
    int  tiff_file_vector_it = -1;
private:
    QPoint move_press_value;
    QPoint move_value;
    bool move_left_is_clicked = false;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
private:
    void set_title_file(const std::string& file_name);
    void set_param_reset();
    bool load_tiff_file(const std::string& file);
private slots:
    void on_pb_exit_clicked();
    void on_pb_mini_clicked();
    void on_pb_max_clicked();
    void on_pb_open_clicked();
    void on_pb_about_clicked();
    void on_pb_left_back_clicked();
    void on_pb_right_forward_clicked();
};
