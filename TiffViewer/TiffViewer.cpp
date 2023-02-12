#include "TiffViewer.h"
// qt
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QFontDatabase>
#include <QFileDialog>
#include <QDesktopWidget>
#ifndef USED_DWM_EDDECT
#include <QGraphicsDropShadowEffect>
#else
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif
// opencv
#include <opencv2/opencv.hpp>
// utils
#include "ConvertUtils.h"

TiffViewer::TiffViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	
#pragma region 窗口设置
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setWindowIcon(QIcon(":/TiffViewer/icon.ico"));
	
	// 添加阴影
#ifndef USED_DWM_EDDECT
	QGraphicsDropShadowEffect* title_shadow = new QGraphicsDropShadowEffect();
	title_shadow->setOffset(0, 4);
	title_shadow->setColor(QColor(0, 0, 0));
	title_shadow->setBlurRadius(12);
	ui.widget_title->setGraphicsEffect(title_shadow);
	QGraphicsDropShadowEffect* client_shadow = new QGraphicsDropShadowEffect();
	client_shadow->setOffset(0, 4);
	client_shadow->setColor(QColor(0, 0, 0));
	client_shadow->setBlurRadius(12);
	ui.widget_client->setGraphicsEffect(client_shadow);
#else
	DWM_BLURBEHIND bb = { 0 };
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = TRUE;
	bb.hRgnBlur = NULL;
	DwmEnableBlurBehindWindow((HWND)winId(), &bb);
#endif	
	
	// 添加渲染区域
	render_label = new RenderLabel(ui.widget_render);
	render_label->setObjectName(QString::fromUtf8("render_label"));
	ui.gridLayout_2->addWidget(render_label, 2, 0, 1, 1);
	
	// 添加可拖动边框
	border_manage = new BorderManage(this, 200, 200, 5);

	// 添加字体
	int text_font = QFontDatabase::addApplicationFont(":/font/Segoe UI.ttf");
	QStringList text_font_ids = QFontDatabase::applicationFontFamilies(text_font);
	if (!text_font_ids.isEmpty())
	{
		QFont font(text_font_ids.first());
		this->setFont(font);
	}
	int icon_font = QFontDatabase::addApplicationFont(":/font/Segoe Fluent Icons.ttf");
	QStringList icon_font_ids = QFontDatabase::applicationFontFamilies(icon_font);
	if (!icon_font_ids.isEmpty()) {
		QFont font(icon_font_ids.first());
		ui.pb_exit->setFont(font);
		ui.pb_mini->setFont(font);
		ui.pb_max->setFont(font);
	}
	
#pragma endregion
	
	// 放大缩小
	connect(ui.pb_zoom_in, &QPushButton::clicked, render_label, &RenderLabel::set_zoom_in);
	connect(ui.pb_zoom_out, &QPushButton::clicked, render_label, &RenderLabel::set_zoom_out);
	connect(ui.pb_zoom_reset, &QPushButton::clicked, render_label, &RenderLabel::set_zoom_reset);
	
	
	// 亮度拖动条
	connect(ui.slider_brightness, &QSlider::valueChanged, render_label, &RenderLabel::set_brightness);
	// 对比度拖动条
	connect(ui.slider_contrast, &QSlider::valueChanged, render_label, &RenderLabel::set_contrast);
	// 反色按钮
	connect(ui.pb_inverse_color, &QPushButton::clicked, render_label, &RenderLabel::set_inverse_color);
	
	// 双击画面打开文件
	connect(render_label, &RenderLabel::signal_open_file, this, &TiffViewer::on_pb_open_clicked);
}

TiffViewer::~TiffViewer()
{}

#pragma region 鼠标事件
void TiffViewer::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (ui.label_title->geometry().contains(event->pos() - QPoint(8,8)))
		{
			move_left_is_clicked = true;
			move_press_value = event->globalPos() - this->pos();
		}
		event->accept();
	}
}

void TiffViewer::mouseMoveEvent(QMouseEvent* event)
{
	if (move_left_is_clicked)
	{
		this->move(event->globalPos() - move_press_value);
		event->accept();
	}
}

void TiffViewer::mouseReleaseEvent(QMouseEvent* event)
{
	move_left_is_clicked = false;
}
void TiffViewer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(255, 255, 255));
	painter.drawRoundedRect(QRect(8, 8, this->width() - 16, this->height() - 16), 8, 8);
}
#pragma endregion

void TiffViewer::set_title_file(const std::string& file)
{
	static QString title = "TiffViewer - ";
	auto file_name = std::filesystem::path(file).filename().string();
	ui.label_title->setText(title + QString::fromLocal8Bit(file_name.c_str()));
}

void TiffViewer::set_param_reset()
{
	ui.slider_brightness->setValue(0);
	ui.slider_contrast->setValue(100); // TODO: 此处的100是1.0的倍数，小数部分整形，待重构
	if (ui.pb_inverse_color->isChecked())
		ui.pb_inverse_color->click();
}

bool TiffViewer::load_tiff_file(const std::string& file)
{
	bool is_load_success = false;

	cv::Mat tiff_origin_mat;
	cv::Mat tiff_8bit_mat;
	try
	{
		// open tiff file
		tiff_origin_mat = cv::imread(file, -1);
		is_load_success = true;
	}
	catch (std::exception& e)
	{
		QDialog* dialog = new QDialog(this);
		dialog->setWindowTitle("Error");

		QLabel* label = new QLabel(dialog);
		std::string msg = "load tiff false, exception:\n" + std::string(e.what());
		label->setText(msg.c_str());
		label->setGeometry(0, 0, 300, 100);
		label->setAlignment(Qt::AlignCenter);

		dialog->show();	
		dialog->setAttribute(Qt::WA_DeleteOnClose);
	}
	if (is_load_success == false)
	{
		return false;
	}

	// convert to 8bit
	tiff_8bit_mat = ConvertUtils::tiff_to_8bit(tiff_origin_mat);
	
	// load dir tiffs
	auto [file_vector, file_vector_it] = ConvertUtils::get_tiff_file_vector(file);
	if (file_vector.size() == 0 || file_vector_it == -1)
	{
		return false;
	}

	tiff_file_vector = file_vector;
	tiff_file_vector_it = file_vector_it;
	
	//---- UI变化
	set_title_file(file);
	set_param_reset();
	//----------

	render_label->set_render_mat(tiff_8bit_mat);
	return true;
}

void TiffViewer::on_pb_exit_clicked()
{
	close();
}
void TiffViewer::on_pb_mini_clicked()
{
	showMinimized();
}

void TiffViewer::on_pb_max_clicked()
{
	static bool is_max = false;
	static QRect normal_rect = this->geometry();
	if (is_max)
	{
		showNormal();
		this->setGeometry(normal_rect);
		is_max = false;
	}
	else
	{
		normal_rect = this->geometry();
		showMaximized();
		auto rect = QApplication::desktop()->availableGeometry();
		rect = rect.marginsRemoved(QMargins(-8, -8,-8, -8));
		setGeometry(rect);
		is_max = true;
	}
}

void TiffViewer::on_pb_open_clicked()
{
	// open dir file
	QString file = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Image Files(*.tif *.tiff)"));
	if (file.isEmpty())
	{
		return;
	}
	
	auto res = load_tiff_file((const char*)file.toLocal8Bit());
	if (res == false)
	{
		return;
	}
}

void TiffViewer::on_pb_about_clicked()
{
	QDialog* dialog = new QDialog(this);
	dialog->setWindowTitle("About");

	QLabel* label = new QLabel(dialog);
	// 从qrc加载txt
	QFile file(":/TiffViewer/LICENSE");
	file.open(QIODevice::ReadOnly);
	QString license = file.readAll();
	file.close();
	label->setText("TiffViewer\n\nAuthor: gengkl200@163.com\n\nVersion: 1.0.0\n\n" + license);
	label->setGeometry(0, 0, 500, 500);
	label->setAlignment(Qt::AlignCenter);

	dialog->show();
	dialog->setAttribute(Qt::WA_DeleteOnClose);
}

void TiffViewer::on_pb_left_back_clicked()
{
	if (tiff_file_vector_it == -1)
	{
		return;
	}
	
	if (tiff_file_vector_it == 0)
	{
		if (is_endless_loop == false)
		{
			return;
		}
		tiff_file_vector_it = tiff_file_vector.size() - 1;
	}
	else
	{
		tiff_file_vector_it = tiff_file_vector_it - 1;
	}

	auto res = load_tiff_file(tiff_file_vector[tiff_file_vector_it]);
	if (res == false)
	{
		return;
	}
}

void TiffViewer::on_pb_right_forward_clicked()
{
	if (tiff_file_vector_it == -1)
	{
		return;
	}

	if (tiff_file_vector_it == tiff_file_vector.size() - 1)
	{
		if (is_endless_loop == false)
		{
			return;
		}
		tiff_file_vector_it = 0;
	}
	else
	{
		tiff_file_vector_it = tiff_file_vector_it + 1;
	}

	auto res = load_tiff_file(tiff_file_vector[tiff_file_vector_it]);
	if (res == false)
	{
		return;
	}
}

