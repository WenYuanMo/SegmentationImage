#ifndef OPENCVSEGMENTATIONIMAGE_H
#define OPENCVSEGMENTATIONIMAGE_H

#include <QObject>
#include<QImage>

#include <opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\types_c.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class opencvSegmentationImage : public QObject
{

    Q_OBJECT
public:
    explicit opencvSegmentationImage(QObject *parent = nullptr);
     ~opencvSegmentationImage();

    static  opencvSegmentationImage* getStaitc()
    {
        if(_opSegImg==nullptr)
        {
            _opSegImg=new opencvSegmentationImage();
        }
        return _opSegImg;
    }
    static void deleteStatic()
    {
        if(_opSegImg!=nullptr)
        {
            delete _opSegImg;
        }
    }

    /**
@brief 初始化
*/
    bool Init();
    /**
@brief 设置灰度值范围
*/
    void SetGrayThrValue(double,double);
    /**
@brief 获取最大灰度分割值
*/
    double GrayThrMaxValue();
    /**
@brief 获取最小灰度分割值
*/
    double GrayThrMinValue();
    /**
@brief 设置图片
*/
    void SetImage(QImage image_);
    /**
@brief 获取处理的结果图片
*/
    QImage GetResultImage();
    /**
@brief 分割图片
*/
    void Segmentation();
    /**
@brief 分割图片测试1
*/
    void SegmentationTest1();
    /**
@brief 分割图片测试2
*/
    void SegmentationTest2();

//signals:

public:

    /**
@brief 原图
*/
    cv::Mat _source_image;
    /**
@brief 结果图片
*/
    cv::Mat _target_image;

private:
    /**
@brief 静态变量
*/
    static opencvSegmentationImage* _opSegImg;

    double _gray_thr_max=255;

    double _gray_thr_min=0;

};

#endif // OPENCVSEGMENTATIONIMAGE_H
