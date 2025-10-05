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

    // Project Version 版本信息
    inline string version                        =   "v1.0.2";

    // Debug Switch 调试开关
    inline bool isDebug                          =   true;
    inline bool isMonitor                        =   true;

    // File Path option 路径相关
    inline std::string Engine_Path               =   "/home/valmorx/CLionProjects/Yolo_AnomalyDetect/models/yolo11s-pose-v2.engine";

    // Engine option 引擎相关
    inline int max_batch_size                    =   1;
    inline int engine_width                      =   640;
    inline int engine_height                     =   640;

    // Camera option 相机相关
    inline int cap_width                         =   1920;
    inline int cap_height                        =   1080;
    inline int cap_index                         =   4;  // 摄像头选择
    inline int roi_width                         =   1920;
    inline int roi_height                        =   1080;

    // Tracker option 追踪器相关
    inline int tracker_frameRate                 =   60; // 追踪器帧率
    inline int tracker_bufferSize                =   30; // 追踪器缓冲区大小

    // Algorithm option 算法相关
    inline int KPS_num                           =   17; // 关键点数量
    inline float KPS_threshold                   =   0.0; // 关键点置信度阈值
    inline float Rect_WidthHeight_Ratio          =   0.9; // 检测框外接矩形长宽比
    inline float MIN_KneeHip_Theta               =   30.0; // 膝盖和脚踝角度最小阈值
    inline float MAX_KneeHip_Theta               =   70.0; // 膝盖和脚踝角度最大阈值
    inline float ShoulderBackKnee_Theta          =   70.0; // 肩、髋、膝夹角阈值
    inline float BackKneeFoot_Theta              =   30.0; // 髋、膝、脚踝夹角阈值

    // Basic Class Tag 原始种类标签
    inline std::vector<std::string> class_names  =   {

        "bending", "down", "up"

    };

    const std::vector<std::vector<unsigned int>> KPS_COLORS =
    {
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255}
    };

    const std::vector<std::vector<unsigned int>> SKELETON =
    {
        {16, 14},
        {14, 12},
        {17, 15},
        {15, 13},
        {12, 13},
        {6,  12},
        {7,  13},
        {6,  7},
        {6,  8},
        {7,  9},
        {8,  10},
        {9,  11},
        {2,  3},
        {1,  2},
        {1,  3},
        {2,  4},
        {3,  5},
        {4,  6},
        {5,  7}
    };

    const std::vector<std::vector<unsigned int>> LIMB_COLORS =
    {
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255},
        {51,  153, 255},
        {255, 51,  255},
        {255, 51,  255},
        {255, 51,  255},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {255, 128, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0},
        {0,   255, 0}
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