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

    int fps_display(cv::Mat& image);
    int fps_display_multiBatch(cv::Mat& image, int batch_size, std::chrono::time_point<std::chrono::high_resolution_clock> batch_start_time);

}

namespace func {

    int anomaly_detect();
    int anomaly_detect_multiBatch();


}

#endif //YOLO_ANOMALYDETECT_FUNC_HPP