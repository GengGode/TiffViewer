#include "BorderManage.h"

BorderManage::BorderManage(QWidget* parent, int min_window_width, int min_window_height, int border_size, int inside_border_depth)
{
	this->parent_widget = parent;
	this->min_window_width = min_window_width;
	this->min_window_height = min_window_height;
	this->border_size = border_size;
	this->inside_border_depth = inside_border_depth;	
	this->setParent(parent);
	init_border();	
}

BorderManage::~BorderManage()
{}

void BorderManage::init_border()
{
    border_left   = new Border(parent_widget, left_border  , this);
    border_right  = new Border(parent_widget, right_border , this);
    border_top    = new Border(parent_widget, top_border   , this);
    border_bottom = new Border(parent_widget, bottom_border, this);
    border_left   ->setStyleSheet("QLabel {background-color:transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
    border_right  ->setStyleSheet("QLabel {background-color:transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
    border_top    ->setStyleSheet("QLabel {background-color:transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
    border_bottom ->setStyleSheet("QLabel {background-color:transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
    border_left   ->raise();
    border_right  ->raise();
    border_top    ->raise();
    border_bottom ->raise();
    
	border_lt  = new Border(parent_widget, left_top_border    , this);
	border_lb  = new Border(parent_widget, left_bottom_border , this);
	border_rt  = new Border(parent_widget, right_top_border   , this);
	border_rb  = new Border(parent_widget, right_bottom_border, this);
	border_lt->setStyleSheet("QLabel {background-color: transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
	border_lb->setStyleSheet("QLabel {background-color: transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
	border_rt->setStyleSheet("QLabel {background-color: transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
	border_rb->setStyleSheet("QLabel {background-color: transparent;}");//ÉèÖÃ±³¾°Í¸Ã÷
    border_lt ->raise();
    border_lb ->raise();
    border_rt ->raise();
    border_rb ->raise();
    
    update_border();
}

void BorderManage::set_min_window_size(int min_window_width, int min_window_height)
{
	this->min_window_width = min_window_width;
	this->min_window_height = min_window_height;
}

void BorderManage::set_border_size(int border_size)
{
	this->border_size = border_size;
}

void BorderManage::set_inside_border_depth(int inside_border_depth)
{
	this->inside_border_depth = inside_border_depth;
}

void BorderManage::update_border()
{
	const int parent_width = parent_widget->width();
	const int parent_height = parent_widget->height();
	const int size = border_size;
	const int depth = this->inside_border_depth;
	border_left->setGeometry(0 + depth, 0+ size + depth, size, parent_height - 2 * size - 2 * depth);
	border_right->setGeometry(parent_width - size - depth, 0 + size + depth, size, parent_height - 2* size - 2 * depth);
	border_top    ->setGeometry(0 + size + depth, 0 + depth, parent_width - 2 * size - 2 * depth, size);
	border_bottom ->setGeometry(0 + size + depth, parent_height - size - depth, parent_width - 2 * size - 2 * depth, size);
	border_lt->setGeometry(0 + depth, 0 + depth, size, size);
	border_lb->setGeometry(0 + depth, parent_height - size - depth, size, size);
	border_rt->setGeometry(parent_width - size - depth, 0 + depth, size, size);
	border_rb->setGeometry(parent_width - size - depth, parent_height - size - depth, size, size);
	parent_widget->update();
}
void BorderManage::set_hide()
{
}
void BorderManage::set_show()
{
}
void BorderManage::get_left_scale_param(QPoint point)
{
	if ((parent_widget->pos().x() + point.x()) > (parent_widget->pos().x() + parent_widget->width() - min_window_width))
	{
		return;
	}
	int x = parent_widget->pos().x() + point.x();
	int y = parent_widget->pos().y();
	int w = parent_widget->width() - point.x();
	int h = parent_widget->height();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_right_scale_param(QPoint point)
{
	if ((parent_widget->pos().x() + parent_widget->width() + point.x()) < (parent_widget->pos().x() + min_window_width))
	{
		return;
	}
	int x = parent_widget->pos().x();
	int y = parent_widget->pos().y();
	int w = parent_widget->width() + point.x();
	int h = parent_widget->height();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_top_scale_param(QPoint point)
{
	if ((parent_widget->pos().y() + point.y()) > (parent_widget->pos().y() + parent_widget->height() - min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x();
	int y = parent_widget->pos().y() + point.y();
	int w = parent_widget->width();
	int h = parent_widget->height() - point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();	
}

void BorderManage::get_bottom_scale_param(QPoint point)
{
	if ((parent_widget->pos().y() + parent_widget->height() + point.y()) < (parent_widget->pos().y() + min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x();
	int y = parent_widget->pos().y();
	int w = parent_widget->width();
	int h = parent_widget->height() + point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_lt_scale_para(QPoint point)
{
	if ((parent_widget->pos().x() + point.x()) > (parent_widget->pos().x() + parent_widget->width() - min_window_width)
		|| (parent_widget->pos().y() + point.y()) > (parent_widget->pos().y() + parent_widget->height() - min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x() + point.x();
	int y = parent_widget->pos().y() + point.y();
	int w = parent_widget->width() - point.x();
	int h = parent_widget->height() - point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_lb_scale_param(QPoint point)
{
	if ((parent_widget->pos().x() + point.x()) > (parent_widget->pos().x() + parent_widget->width() - min_window_width)
		|| (parent_widget->pos().y() + parent_widget->height() + point.y()) < (parent_widget->pos().y() + min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x() + point.x();
	int y = parent_widget->pos().y();
	int w = parent_widget->width() - point.x();
	int h = parent_widget->height() + point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_rt_scale_param(QPoint point)
{
	if ((parent_widget->pos().x() + parent_widget->width() + point.x()) < (parent_widget->pos().x() + min_window_width)
		|| (parent_widget->pos().y() + point.y()) > (parent_widget->pos().y() + parent_widget->height() - min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x();
	int y = parent_widget->pos().y() + point.y();
	int w = parent_widget->width() + point.x();
	int h = parent_widget->height() - point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}

void BorderManage::get_rb_scale_param(QPoint point)
{
	if ((parent_widget->pos().x() + parent_widget->width() + point.x()) < (parent_widget->pos().x() + min_window_width)
		|| (parent_widget->pos().y() + parent_widget->height() + point.y()) < (parent_widget->pos().y() + min_window_height))
	{
		return;
	}
	int x = parent_widget->pos().x();
	int y = parent_widget->pos().y();
	int w = parent_widget->width() + point.x();
	int h = parent_widget->height() + point.y();
	parent_widget->setGeometry(x, y, w, h);
	update_border();
}
