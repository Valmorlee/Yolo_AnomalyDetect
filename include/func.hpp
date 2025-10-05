//
// Created by valmorx on 2025/10/5.
//

#ifndef YOLO_ANOMALYDETECT_FUNC_HPP
#define YOLO_ANOMALYDETECT_FUNC_HPP

#include <BYTETracker.h>
#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
#include "trtyolo.hpp"

#include "params.hpp"

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

namespace tools {
    // bool isTracking(int classId);                                                        // 判断是否为追踪目标
    base::dataBox filterBoxes(std::vector<base::dataBox> &boxes);
    void drawRes(cv::Mat &input, base::dataBox &output, Scalar &color);
    // void drawRes_tracker(cv::Mat &input, std::vector<STrack> &output);

    void draw_KPS_objects(
        const cv::Mat &image,
        cv::Mat &res,
        const std::vector<Object> &objs,
        const std::vector<std::vector<unsigned int>> &SKELETON,
        const std::vector<std::vector<unsigned int>> &KPS_COLORS,
        const std::vector<std::vector<unsigned int>> &LIMB_COLORS
    );

    cv::Rect get_centerRect(cv::Point2f center, float width, float height);                    // 获取以定点作为矩形中心点的矩形
    std::vector<base::dataBox> revert2Box(const trtyolo::PoseRes &res);               // yoloBox转换为dataBox | 数据格式转换
    std::vector<Object> revert2Tracker(const trtyolo::PoseRes &res);                    // yoloBox转换为bytetracker | 数据格式转换
    std::vector<base::dataBox> tracker2Box(vector<STrack> &output);                    // bytetracker转换为dataBox | 数据格式转换
    bool fallDetXAlgorithm(const std::vector<float>& kps);

    void detect_By_tracker(
        const cv::Mat &image,
        cv::Mat &res,
        const std::vector<Object> &objs,
        const std::vector<STrack>& output_stracks,
        const std::vector<std::vector<unsigned int>> &SKELETON,
        const std::vector<std::vector<unsigned int>> &KPS_COLORS,
        const std::vector<std::vector<unsigned int>> &LIMB_COLORS
    );

}

namespace function {

}

#endif //YOLO_ANOMALYDETECT_FUNC_HPP