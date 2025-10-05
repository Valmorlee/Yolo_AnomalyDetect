//
// Created by valmorx on 2025/10/5.
//

#ifndef YOLO_ANOMALYDETECT_PARAMS_HPP
#define YOLO_ANOMALYDETECT_PARAMS_HPP


#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "trtyolo.hpp"

namespace params {

    // Debug Switch 调试开关
    inline bool isDebug                          =   true;
    inline bool isMonitor                        =   true;

    // File Path option 路径相关
    inline std::string Engine_Path               =   "../models/yolo11s-pose-v2.engine";
    inline std::string Video_Path                =   "/home/valmorx/DeepLearningSource/Meow.mp4";

    // Engine option 引擎相关
    inline int max_batch_size                    =   1;
    inline int engine_width                      =   320;
    inline int engine_height                     =   320;

    // Camera option 相机相关
    inline int cap_width                         =   1920;
    inline int cap_height                        =   1080;
    inline int cap_index                         =   0;  // 摄像头选择
    inline int roi_width                         =   1920;
    inline int roi_height                        =   1080;
    inline int tracker_frameRate                 =   60; // 追踪器帧率
    inline int tracker_bufferSize                =   30; // 追踪器缓冲区大小

    // Basic Class Tag 原始种类标签
    inline std::vector<std::string> class_names  =   {

        "bending", "down", "up"

    };


}

namespace base {

    inline cv::Mat roiImg; // ROI 图像

    // 待初始化参数
    inline trtyolo::InferOption option;
    inline std::unique_ptr<trtyolo::DetectRes> detector;
    inline cv::VideoCapture cap;

    inline std::chrono::time_point<std::chrono::system_clock> start;
    inline std::chrono::time_point<std::chrono::system_clock> end;

    //标准yolo输出数据类
    class dataBox {
    public:

        //边界版
        float leftBound;
        float rightBound;
        float topBound;
        float bottomBound;

        //坐标版
        cv::Point2f leftUp;
        float width;
        float height;

        cv::Point2f centerPoint;
        float prob;
        int classId;

        int ExistTag;

        dataBox(float leftBound, float rightBound, float topBound, float bottomBound, float prob, int classId) {
            this->leftBound = leftBound;
            this->rightBound = rightBound;
            this->topBound = topBound;
            this->bottomBound = bottomBound;
            this->leftUp = cv::Point(leftBound, topBound);
            this->width = rightBound - leftBound;
            this->height = bottomBound - topBound;
            this->centerPoint = cv::Point2f(leftBound + width / 2.0, topBound + height / 2.0);
            this->prob = prob;
            this->classId = classId;
            this->ExistTag = 1;
        }

        dataBox(cv::Point2f leftUp, float width, float height, float prob, int classId) {
            this->leftUp = leftUp;
            this->width = width;
            this->height = height;
            this->leftBound = leftUp.x;
            this->rightBound = leftUp.x + width;
            this->topBound = leftUp.y;
            this->bottomBound = leftUp.y + height;
            this->centerPoint = cv::Point2f(leftUp.x + width / 2.0, leftUp.y + height / 2.0);
            this->prob = prob;
            this->classId = classId;
            this->ExistTag = 1;
        }

        dataBox(const dataBox &box) {
            this->leftBound = box.leftBound;
            this->rightBound = box.rightBound;
            this->topBound = box.topBound;
            this->bottomBound = box.bottomBound;
            this->leftUp = box.leftUp;
            this->width = box.width;
            this->height = box.height;
            this->centerPoint = box.centerPoint;
            this->prob = box.prob;
            this->classId = box.classId;
            this->ExistTag = box.ExistTag;
        }

        dataBox operator=(const dataBox &box) {
            this->leftBound = box.leftBound;
            this->rightBound = box.rightBound;
            this->topBound = box.topBound;
            this->bottomBound = box.bottomBound;
            this->leftUp = box.leftUp;
            this->width = box.width;
            this->height = box.height;
            this->centerPoint = box.centerPoint;
            this->prob = box.prob;
            this->classId = box.classId;
            this->ExistTag = box.ExistTag;
            return *this;
        }

        dataBox() = default;

    };

    inline dataBox output_dataBox = dataBox(); // 输出数据
    inline std::vector<dataBox> output_dataBoxes = std::vector<dataBox>(); // 输出群数据


}


#endif //YOLO_ANOMALYDETECT_PARAMS_HPP