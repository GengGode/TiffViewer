#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

enum BorderType
{
    no_border = 0,
    left_border,
    right_border,
    top_border,	
    bottom_border,
    left_top_border,	
    left_bottom_border,    
    right_top_border,    
    right_bottom_border,    
};

class BorderManage  : public QObject
{
	Q_OBJECT

public:
	BorderManage(QWidget* parent, int min_window_width = 30 , int min_window_height = 30, int border_size = 5, int inside_border_depth = 8);
	~BorderManage();
public:
	void init_border();
public:
	void set_min_window_size(int min_window_width, int min_window_height); 
	void set_border_size(int border_size);
	void set_inside_border_depth(int inside_border_depth);
	void update_border();
public slots:
    void set_hide();
	void set_show();

private:
    void get_left_scale_param(QPoint point);
    void get_right_scale_param(QPoint point);
    void get_top_scale_param(QPoint point);
    void get_bottom_scale_param(QPoint point);
    void get_lt_scale_para(QPoint point);
    void get_lb_scale_param(QPoint point);
    void get_rt_scale_param(QPoint point);
    void get_rb_scale_param(QPoint point);
    //内部边框类，防止外部创建
    class Border :public QLabel
    {
    public:
        //设置边框属性
        Border(QWidget* parent, BorderType border_type, BorderManage* manager)
        {
			this->setParent(parent);
			this->border_type = border_type;
			this->manager = manager;
            switch (border_type)
            {
            case left_border:
                this->setCursor(Qt::SizeHorCursor);
                break;
            case right_border:
                this->setCursor(Qt::SizeHorCursor);
                break;
            case top_border:
                this->setCursor(Qt::SizeVerCursor);
                break;
            case bottom_border:
                this->setCursor(Qt::SizeVerCursor);
                break;
            case left_top_border:
                this->setCursor(Qt::SizeFDiagCursor);
                break;
            case left_bottom_border:
                this->setCursor(Qt::SizeBDiagCursor);
                break;
            case right_top_border:
                this->setCursor(Qt::SizeBDiagCursor);
                break;
            case right_bottom_border:
                this->setCursor(Qt::SizeFDiagCursor);
                break;
            default:
                break;
            }
        }
    protected:
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
			if (event->button() == Qt::LeftButton)
			{
				is_keep_press = true;
				press_point = event->globalPos();
			}
			return QWidget::mousePressEvent(event);
        }
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
            if (is_keep_press)
            {
				const QPoint move_point = event->globalPos() - press_point;
				switch (border_type)
				{
				case left_border:
					manager->get_left_scale_param(move_point);
					break;
				case right_border:
					manager->get_right_scale_param(move_point);
					break;
				case top_border:
					manager->get_top_scale_param(move_point);
					break;
				case bottom_border:
					manager->get_bottom_scale_param(move_point);
					break;
				case left_top_border:
					manager->get_lt_scale_para(move_point);
					break;
				case left_bottom_border:
					manager->get_lb_scale_param(move_point);
					break;
				case right_top_border:
					manager->get_rt_scale_param(move_point);
					break;
				case right_bottom_border:
					manager->get_rb_scale_param(move_point);
					break;
				default:
					break;
				}
				press_point = event->globalPos();
            }
			return QWidget::mouseMoveEvent(event);
        }
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE
        {
			is_keep_press = false;
			return QWidget::mouseReleaseEvent(event);
        }

    private:
        BorderManage* manager = nullptr; //保存外部类指针
        BorderType border_type;
        QPoint press_point;
        bool is_keep_press = false;
    };

    int border_size; //边框捕获区大小
    int min_window_width; //最小窗口高度
    int min_window_height;   //最小窗口宽度
	int inside_border_depth; //内部边框深度

    QWidget* parent_widget = nullptr;
    //边框控件
    Border* border_left = nullptr;
    Border* border_right = nullptr;
    Border* border_top = nullptr;
    Border* border_bottom = nullptr;
    Border* border_lt = nullptr;
    Border* border_lb = nullptr;
    Border* border_rt = nullptr;
    Border* border_rb = nullptr;
};
