#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ///设置大小
    resize(800, 600);
    ///创建主要窗体主控件
    centralwidget = new QWidget(this);
//    centralwidget->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0)"));
    ///主要的控件布局
    gridLayout =new QGridLayout(centralwidget);
    ///主要的垂直布局
    verticalLayout =new QVBoxLayout(centralwidget);
    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
    ///按钮的横向布局
    horizontalLayout_btn=new QHBoxLayout(centralwidget);
    ///创建加载图片按钮
    loadImageButton=new QPushButton(centralwidget);
//    loadImageButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"
//                                                     "color: rgb(255, 255, 255);"
//                                                     "border-style:outset;"
//                                                     "border-width: 2px;"
//                                                     "border-color: rgb(200, 200, 200);"
//                                                     "radius:2px"));
    loadImageButton->setMinimumSize(QSize(150,50));
    loadImageButton->setMaximumHeight(50);
    loadImageButton->setText(QString("加载图片"));
    horizontalLayout_btn->addWidget(loadImageButton);
    ///分割图片
    segmentationButton=new QPushButton(centralwidget);
//    segmentationButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"
//                                                     "color: rgb(255, 255, 255);"
//                                                     "border-style:outset;"
//                                                     "border-width: 2px;"
//                                                     "border-color: rgb(200, 200, 200);"));
    segmentationButton->setMinimumSize(QSize(150,50));
    segmentationButton->setMaximumHeight(50);
    segmentationButton->setText(QString("分割图片"));
    horizontalLayout_btn->addWidget(segmentationButton);

    ///按钮横向布局加入垂直布局
    verticalLayout->addLayout(horizontalLayout_btn);

    ///创建水平布局widget控件
    horizontalLayout_Widget=new QHBoxLayout(centralwidget);

    ///图片操作控件
    imageWidget =new ImageWidget(centralwidget);
//    imageWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
    horizontalLayout_Widget->addWidget(imageWidget);

    //图片处理结果控件
    resultImageWidget=new ImageWidget(centralwidget);
//    resultImageWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
    horizontalLayout_Widget->addWidget(resultImageWidget);

    paramterGroupBox=new QGroupBox(centralwidget);
    paramterGroupBox->setTitle(QString("参数"));
//    paramterGroupBox->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"
//                                                      "color: rgb(255, 255, 255);"));
    paramterGroupBox->setMaximumWidth(300);
    paramterGroupBox->setMinimumWidth(300);

    gridLayoutParamterGroupBox=new QGridLayout(paramterGroupBox);

    paramterTabWidget =new QTabWidget(paramterGroupBox);
//    paramterTabWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"
//                                                      "color: rgb(0, 0, 0);"));
    imageParamterWidget=new QWidget(paramterGroupBox);

    imageParamterFormLayout=new QFormLayout(paramterGroupBox);

    grayThrMaxLabel=new QLabel(paramterGroupBox);
    grayThrMaxLabel->setText("最大灰度值");
    imageParamterFormLayout->setWidget(0,QFormLayout::LabelRole,grayThrMaxLabel);

    grayThrMaxSpinBox=new QSpinBox(paramterGroupBox);
    grayThrMaxSpinBox->setMaximum(255);
    grayThrMaxSpinBox->setMinimum(0);
    grayThrMaxSpinBox->setSingleStep(1);
    grayThrMaxSpinBox->setValue(30);
    imageParamterFormLayout->setWidget(0,QFormLayout::FieldRole,grayThrMaxSpinBox);

    grayThrMinLabel=new QLabel(paramterGroupBox);
    grayThrMinLabel->setText("最小灰度值");
    imageParamterFormLayout->setWidget(1,QFormLayout::LabelRole,grayThrMinLabel);

    grayThrMinSpinBox=new QSpinBox(paramterGroupBox);
    grayThrMinSpinBox->setMaximum(255);
    grayThrMinSpinBox->setMinimum(0);
    grayThrMinSpinBox->setSingleStep(1);
    grayThrMinSpinBox->setValue(0);
    imageParamterFormLayout->setWidget(1,QFormLayout::FieldRole,grayThrMinSpinBox);

    imageParamterWidget->setLayout(imageParamterFormLayout);///加入图片参数widget

    paramterTabWidget->addTab(imageParamterWidget,QString::fromUtf8("分割参数"));

    gridLayoutParamterGroupBox->addWidget(paramterTabWidget,0,0,1,1);

    paramterGroupBox->setLayout(gridLayoutParamterGroupBox);

    horizontalLayout_Widget->addWidget(paramterGroupBox);

    verticalLayout->addLayout(horizontalLayout_Widget);

    ///设置窗口主要显示
    this->setCentralWidget(centralwidget);

    ///初始化图片处理工具
    opencvSegmentationImage::getStaitc()->Init();

    grayThrMaxSpinBox->setValue(opencvSegmentationImage::getStaitc()->GrayThrMaxValue());
    grayThrMinSpinBox->setValue(opencvSegmentationImage::getStaitc()->GrayThrMinValue());

    ///链接信号
    connect(loadImageButton,SIGNAL(clicked()), this, SLOT(LoadImage()));
    connect(segmentationButton,SIGNAL(clicked()), this, SLOT(SegmentationImage()));

    connect(grayThrMaxSpinBox,SIGNAL(valueChanged(int)),this,SLOT(GrayThrMaxChangle(int)));
    connect(grayThrMinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(GrayThrMinChangle(int)));

}

MainWindow::~MainWindow()
{
}

void MainWindow::LoadImage()
{
    QString curPath=QDir::currentPath();//获取系统当前目录

    //QString dlgTitle=QString::fromLocal8Bit("选择一个文件");  中文会编译不过
    //QString filter=QString::fromLocal8Bit("图片文件(*.jpg *.gif *.png *.bmp)"); //文件过滤器

    QString dlgTitle="choice one image";
    QString filter="image(*.jpg *.gif *.png *.bmp)"; //文件过滤器

    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,"",filter);

    if(aFileName!="")
    {
        imageWidget->setImage(aFileName);
        imageWidget->fitWindow();
    }
}

void MainWindow::SegmentationImage()
{
    if(imageWidget->_imageEixt==true)
    {
        ///获取参数
        opencvSegmentationImage::getStaitc()->SetImage(imageWidget->getImage());

        int gray_thr_max_=grayThrMaxSpinBox->value();
        int gray_thr_min_=grayThrMinSpinBox->value();
        opencvSegmentationImage::getStaitc()->SetGrayThrValue(gray_thr_max_,gray_thr_min_);

        ///分析
        opencvSegmentationImage::getStaitc()->Segmentation();

        ///显示结果
        resultImageWidget->setImage(opencvSegmentationImage::getStaitc()->GetResultImage());
    }
}

void MainWindow::GrayThrMaxChangle(int value_)
{
    int gray_thr_min_=grayThrMinSpinBox->value();

    if(value_>gray_thr_min_)
    {
        SegmentationImage();
    }
    else
    {
        grayThrMaxSpinBox->setValue(gray_thr_min_+1);
    }
}

void MainWindow::GrayThrMinChangle(int value_)
{
    int gray_thr_max_=grayThrMaxSpinBox->value();

    if(value_<gray_thr_max_)
    {
        SegmentationImage();
    }
    else
    {
        grayThrMinSpinBox->setValue(gray_thr_max_-1);
    }
}
