#include "opencvsegmentationimage.h"

/**
@brief 创建静态变量
*/
opencvSegmentationImage* opencvSegmentationImage::_opSegImg=nullptr;

opencvSegmentationImage::opencvSegmentationImage(QObject *parent) : QObject(parent)
{
    _source_image=cv::Mat(0,0,CV_8UC1);///创建一个空的矩阵
    _target_image=cv::Mat(0,0,CV_8UC1);///创建一个空的矩阵
    _gray_thr_min=0;
    _gray_thr_max=100;
}

opencvSegmentationImage::~opencvSegmentationImage()
{
    _source_image.release();
    _target_image.release();
}

bool opencvSegmentationImage::Init()
{
    return true;
}

void opencvSegmentationImage::SetGrayThrValue(double gray_thr_max_, double gray_thr_min_)
{
    _gray_thr_max=gray_thr_max_;
    _gray_thr_min=gray_thr_min_;
}

double opencvSegmentationImage::GrayThrMaxValue()
{
    return _gray_thr_max;
}

double opencvSegmentationImage::GrayThrMinValue()
{
    return _gray_thr_min;
}

void opencvSegmentationImage::SetImage(QImage image_)
{
    Mat mat = Mat::zeros(image_.height(), image_.width(),image_.format()); ///初始化Mat

    switch(image_.format()) ///判断image的类型
    {
    case QImage::Format_Grayscale8:  ///灰度图
        mat = Mat(image_.height(), image_.width(),
                  CV_8UC1,(void*)image_.constBits(),image_.bytesPerLine());
        break;
    case QImage::Format_RGB888: ///3通道彩色
        mat = Mat(image_.height(), image_.width(),
                  CV_8UC3,(void*)image_.constBits(),image_.bytesPerLine());
        break;
    case QImage::Format_RGB32:
        mat = Mat(image_.height(), image_.width(),
                  CV_8UC4,(void*)image_.constBits(),image_.bytesPerLine());
        break;

    case QImage::Format_ARGB32: ///4通道彩色 a通道为透明通道
        mat = Mat(image_.height(), image_.width(),
                  CV_8UC4,(void*)image_.constBits(),image_.bytesPerLine());
        break;

    case QImage::Format_Indexed8:///单通道
        mat = cv::Mat(image_.height(), image_.width(),
                             CV_8UC1, (void*)image_.bits(), image_.bytesPerLine());
        break;

    default:
        ;
    }
    _source_image.release();
    _source_image=mat.clone();

//    cv::namedWindow("sourceImage",0);
//    cv::resizeWindow("sourceImage", 640, 480);
//    cv::imshow("sourceImage",_source_image);
}

QImage opencvSegmentationImage::GetResultImage()
{
    //判断m的类型，可能是CV_8UC1  CV_8UC2  CV_8UC3  CV_8UC4
    switch(_target_image.type())
    {
    //QIamge 构造函数, ((const uchar *data, 宽(列),高(行), 一行共多少个（字节）通道，宽度*字节数，宏参数)
    case CV_8UC1:
    {
        QImage img((uchar *)_target_image.data,_target_image.cols,_target_image.rows,_target_image.cols * 1,QImage::Format_Grayscale8);
        return img;
    }
        break;
    case CV_8UC3:   //一个像素点由三个字节组成
    {
        //cvtColor(m,m,COLOR_BGR2RGB); BGR转RGB
        QImage img((uchar *)_target_image.data,_target_image.cols,_target_image.rows,_target_image.cols * 3,QImage::Format_RGB888);
        return img.rgbSwapped(); //opencv是BGR  Qt默认是RGB  所以RGB顺序转换
    }
        break;
    case CV_8UC4:
    {
//        QImage img((uchar *)_target_image.data,_target_image.cols,_target_image.rows,_target_image.cols * 4,QImage::Format_RGBA8888);
         QImage img((uchar *)_target_image.data,_target_image.cols,_target_image.rows,_target_image.cols * 4,QImage::Format_RGB32);
        return img;
    }
        break;
    default:
    {
        QImage img; //如果遇到一个图片均不属于这三种，返回一个空的图片
        return img;
    }
    }
}

void opencvSegmentationImage::Segmentation()
{
    _target_image.release();
    if(_source_image.rows>0&&_source_image.cols>0)
    {
        ///把图片转成灰度图片
        cv::Mat gray_image_,
                mask_image_,
                image_check_;

        ///roi参数
        double x1_=0;
        double y1_=0;
        double x2_=_source_image.cols;
        double y2_=_source_image.rows;

        int source_image_channels_=_source_image.channels();
        ///把图片转成灰度图片
        if(source_image_channels_!=1)
        {
            cv::cvtColor(_source_image,gray_image_,cv::COLOR_BGR2GRAY);
        }
        else
        {
            _source_image.copyTo(gray_image_);
        }

        ///获取感兴趣区域
        mask_image_=cv::Mat::zeros(gray_image_.size(),CV_8UC1);
        cv::Rect r1_(x1_,y1_,x2_,y2_);
        mask_image_(r1_).setTo(255);
        gray_image_.copyTo(image_check_,mask_image_);

        int row1_=image_check_.rows;
        int col1_=image_check_.cols*image_check_.channels();
        for(int r_=0;r_<row1_;r_++)
        {
            uchar* data= image_check_.ptr<uchar>(r_);
            for(int c_=0;c_<col1_;c_++)
            {
                int n_=data[c_];
                if(n_<_gray_thr_max&&n_>_gray_thr_min)
                {
                    data[c_]=255;
                }
                else
                {
                    data[c_]=0;
                }
            }
        }

        // 轮廓查找
        vector<vector<cv::Point>> contours_;
        vector<Vec4i> hierarchy_;
        cv::findContours(image_check_,
                         contours_,
                         CV_RETR_CCOMP,
                         CV_CHAIN_APPROX_NONE);
        //面积选择
        int count_=contours_.size();

        ///判断图片是否为3通道
        if(source_image_channels_==1)
        {
            cv::cvtColor(_source_image,_target_image,cv::COLOR_GRAY2RGB);
        }
        else
        {
            ///复制原图到目标图片
            _source_image.copyTo(_target_image);
        }

        ///显示分析到的轮廓数据
        if(count_>0)
        {
            cv::drawContours(_target_image,
                             contours_,
                             -1,
                             cv::Scalar(0,0,255),
                             2);
        }

        image_check_.release();
        mask_image_.release();
        gray_image_.release();
    }
}

void opencvSegmentationImage::SegmentationTest1()
{
    if(_source_image.rows>0&&_source_image.cols>0)
    {
        _source_image.copyTo(_target_image);

        cv::Mat gray_image_,
                mean_image_,
                diff_image_,
                thr_low_image_,
                thr_hight_image_,
                thr_image_,
                k_opening_,
                k_closing_,
                opening_image_,
                closing_image_;

        // 轮廓查找
        vector<vector<cv::Point>> contours_;
        vector<Vec4i> hierarchy_;

        ///把图片转成灰度图片
        if(_source_image.channels()==3)
        {
            cv::cvtColor(_source_image,gray_image_,cv::COLOR_BGR2GRAY);
        }
        else
        {
            _source_image.copyTo(gray_image_);
        }
        ///模糊图片
        cv::medianBlur(gray_image_,mean_image_,201);
        ///图片相减
        diff_image_=gray_image_-mean_image_;
        ///大于150 为255
        cv::threshold(diff_image_,thr_low_image_,150,255,cv::THRESH_BINARY);
        ///大于220为255
        cv::threshold(diff_image_,thr_hight_image_,220,255,cv::THRESH_BINARY);
        ///相减   得到220 到150 区域
        cv::absdiff(thr_low_image_,thr_hight_image_,thr_image_);
        ///创建开循环数据
        k_opening_=cv::Mat::zeros(18,18,CV_8U);
        cv::circle(k_opening_,
                   cv::Point(8,8),
                   9,
                   cv::Scalar(1,1,1),
                   -1,
                   cv::LINE_AA);
        cv::morphologyEx(thr_image_,
                         opening_image_,
                         cv::MORPH_OPEN,
                         k_opening_);

        ///创建闭循环数据
        k_closing_=cv::Mat::zeros(20,20,CV_8U);
        cv::circle(k_closing_,
                   cv::Point(10,10),
                   10,
                   cv::Scalar(1,1,1),
                   -1,
                   cv::LINE_AA);
        cv::morphologyEx(thr_image_,
                         closing_image_,
                         cv::MORPH_CLOSE,
                         k_closing_);
        //轮廓分析
        cv::findContours(closing_image_,
                         contours_,
                         hierarchy_,
                         cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_NONE);

        int count_=contours_.size();
        ///分析
        for(int i=0;i<count_;i++)
        {
            cv::Rect re_;
            re_= cv::boundingRect(contours_[i]);///最小外接矩形
            double x_,y_,w_,h_;
            x_=re_.x;
            y_=re_.y;
            h_=re_.height;
            w_=re_.width;
            if(w_>5&&h_>5)
            {
            }
        }

        ///显示分析到的轮廓数据
        if(count_>0)
        {
            cv::drawContours(_target_image,
                             contours_,
                             -1,
                             cv::Scalar(0,0,255),
                             2);
        }

        //创建窗体
        cv::namedWindow("target",0);
        cv::resizeWindow("target", 640, 480);
        cv::namedWindow("gray",0);
        cv::resizeWindow("gray", 640, 480);
        cv::namedWindow("mean",0);
        cv::resizeWindow("mean", 640, 480);
        cv::namedWindow("diff",0);
        cv::resizeWindow("diff", 640, 480);
        cv::namedWindow("thres",0);
        cv::resizeWindow("thres", 640, 480);
        cv::namedWindow("opening",0);
        cv::resizeWindow("opening", 640, 480);
        cv::namedWindow("closing",0);
        cv::resizeWindow("closing", 640, 480);


        //显示图片
        cv::imshow("target",_target_image);
        cv::imshow("gray",gray_image_);
        cv::imshow("mean",mean_image_);
        cv::imshow("diff",diff_image_);
        cv::imshow("thres",thr_image_);
        cv::imshow("opening",opening_image_);
        cv::imshow("closing",closing_image_);

        cv::waitKey(0);
//        system("pause");

        cv::destroyWindow("target");
        cv::destroyWindow("gray");
        cv::destroyWindow("mean");
        cv::destroyWindow("diff");
        cv::destroyWindow("thres");
        cv::destroyWindow("opening");
        cv::destroyWindow("closing");

        gray_image_.release();
        mean_image_.release();
        diff_image_.release();
        thr_low_image_.release();
        thr_hight_image_.release();
        thr_image_.release();
        k_opening_.release();
        k_closing_.release();
        opening_image_.release();
        closing_image_.release();
    }
}

void opencvSegmentationImage::SegmentationTest2()
{
    if(_source_image.rows>0&&_source_image.cols>0)
    {
        ///把图片转成灰度图片
        cv::Mat gray_image_,
                mask_image_,
                image_check_;

        ///roi参数
        double x1_=0;
        double y1_=0;
        double x2_=_source_image.cols;
        double y2_=_source_image.rows;

        int source_image_channels_=_source_image.channels();
        ///把图片转成灰度图片
        if(_source_image.channels()!=1)
        {
            cv::cvtColor(_source_image,gray_image_,cv::COLOR_BGR2GRAY);
        }
        else
        {
            _source_image.copyTo(gray_image_);
        }

        ///获取感兴趣区域
        mask_image_=cv::Mat::zeros(gray_image_.size(),CV_8UC1);
        cv::Rect r1_(x1_,y1_,x2_,y2_);
        mask_image_(r1_).setTo(255);
        gray_image_.copyTo(image_check_,mask_image_);

        int row1_=image_check_.rows;
        int col1_=image_check_.cols*image_check_.channels();
        for(int r_=0;r_<row1_;r_++)
        {
            uchar* data= image_check_.ptr<uchar>(r_);
            for(int c_=0;c_<col1_;c_++)
            {
                int n_=data[c_];
                if(n_<_gray_thr_max&&n_>_gray_thr_min)
                {
                    data[c_]=255;
                }
                else
                {
                    data[c_]=0;
                }
            }
        }

        // 轮廓查找
        vector<vector<cv::Point>> contours_;
        vector<Vec4i> hierarchy_;

        cv::findContours(image_check_,
                         contours_,
                         CV_RETR_CCOMP,
                         CV_CHAIN_APPROX_NONE);

        //面积选择
        int count_=contours_.size();
        //        for(int j=0;j<count_;j++)
        //        {
        //            double area_ = cv::contourArea(contours_[j]);
        //            if(area_>_min_area&&area_<_max_area)
        //            {
        //                //轮廓的最小外结矩形
        //                RotatedRect min_rect_ = minAreaRect(contours_[j]);
        //                cv::Size2f size_= min_rect_.size;
        //                if(_min_width<size_.width&&_max_width>size_.width)
        //                {
        //                    if(_min_height<size_.height&&_max_height>size_.height)
        //                    {
        //                        ResultTuple* result_=new ResultTuple();
        //                        result_->_id=j;
        //                        result_->_roi_id=i;
        //                        result_->_area=area_;
        //                        result_->_width=size_.width;
        //                        result_->_height=size_.height;
        //                        result_->_x=min_rect_.center.x;
        //                        result_->_y=min_rect_.center.y;
        //                        _result_tuples.append(result_);
        //                    }
        //                }
        //            }
        //        }

        ///判断图片是否为3通道
        if(_source_image.channels()==1)
        {
            cv::cvtColor(_source_image,_target_image,cv::COLOR_GRAY2RGB);
        }
        else
        {
            ///复制原图到目标图片
            _source_image.copyTo(_target_image);
        }

        ///显示分析到的轮廓数据
        if(count_>0)
        {
            cv::drawContours(_target_image,
                             contours_,
                             -1,
                             cv::Scalar(0,0,255),
                             2);
        }

        //创建窗体
        cv::namedWindow("target",0);
        cv::resizeWindow("target", 640, 480);
        cv::namedWindow("gray",0);
        cv::resizeWindow("gray", 640, 480);

        //显示图片
        cv::imshow("target",_target_image);
        cv::imshow("gray",gray_image_);

//        cv::destroyWindow("target");
//        cv::destroyWindow("gray");

        image_check_.release();
        mask_image_.release();
        gray_image_.release();
    }
}





