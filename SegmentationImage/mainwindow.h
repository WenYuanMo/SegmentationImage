#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <imagewidget.h>
#include <QFileDialog>
#include<QTabWidget>
#include<QFormLayout>
#include<QLabel>
#include<QSpinBox>

#include <opencvsegmentationimage.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    /**
@brief 主要的控件
*/
    QWidget *centralwidget;

    /**
@brief 网格布局
*/
    QGridLayout *gridLayout;

    /**
@brief 主要的垂直布局
*/
    QVBoxLayout *verticalLayout;

    /**
@brief 按钮水平布局
*/
    QHBoxLayout *horizontalLayout_btn;

    /**
@brief 加载图片按钮
*/
    QPushButton *loadImageButton;

    /**
@brief 分割图片
*/
    QPushButton *segmentationButton;

    /**
@brief widget水平布局
*/
    QHBoxLayout *horizontalLayout_Widget;

    /**
@brief 图片显示控件
*/
    ImageWidget  *imageWidget;
    /**
@brief 处理的结果图片
*/
   ImageWidget *resultImageWidget;
   /**
@brief 参数组
*/
   QGroupBox *paramterGroupBox;
   /**
@brief 参数组网格布局
*/
   QGridLayout *gridLayoutParamterGroupBox;
   /**
@brief 参数tabwidget
*/
   QTabWidget *paramterTabWidget;
   /**
@brief 图片参数widget
*/
   QWidget *imageParamterWidget;
   /**
@brief 图片参数formlayout
*/
   QFormLayout *imageParamterFormLayout;
   /**
@brief 灰度值分割最大值
*/
   QLabel *grayThrMaxLabel;
   /**
@brief 灰度值分割最大值
*/
   QSpinBox *grayThrMaxSpinBox;
/**
@brief 灰度值分割最小值
*/
   QLabel *grayThrMinLabel;
   /**
@brief 灰度值分割最小值
*/
   QSpinBox *grayThrMinSpinBox;

public slots:
   /**
@brief 加载一张图片
*/
   void LoadImage();
   /**
@brief 分割图片
*/
   void SegmentationImage();
   /**
@brief 最大灰度值改变
*/
   void GrayThrMaxChangle(int);
   /**
@brief 最小灰度值改变
*/
   void GrayThrMinChangle(int);

};
#endif // MAINWINDOW_H
