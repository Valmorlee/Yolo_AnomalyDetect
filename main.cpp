#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

#include "BYTETracker.h"
#include "func.hpp"
#include "trtyolo.hpp"

int main() {
    try {
        // -------------------- 初始化配置 --------------------
        trtyolo::InferOption option;
        option.enableSwapRB();  // BGR->RGB转换

        // -------------------- 模型初始化 --------------------
        auto detector = std::make_unique<trtyolo::PoseModel>(
            "../models/yolo11s-pose-v2.engine",  // 模型路径
            option                  // 推理设置
        );

        // -------------------- 打开摄像头 --------------------
        cv::VideoCapture cap(4, cv::CAP_V4L2);
        if (!cap.isOpened()) {
            throw std::runtime_error("无法打开摄像头");
        }

        // 设置摄像头参数
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
        cap.set(cv::CAP_PROP_FPS, 60);

        cv::Mat frame;
        std::cout << "按 'q' 键退出程序" << std::endl;

        // 定义人体关键点连接关系（骨架）
        std::vector<std::vector<int>> skeleton = {
            {16, 14}, {14, 12}, {17, 15}, {15, 13}, {12, 13}, {6, 12}, {7, 13},
            {6, 7}, {6, 8}, {7, 9}, {8, 10}, {9, 11}, {2, 3}, {1, 2}, {1, 3},
            {2, 4}, {3, 5}, {4, 6}, {5, 7}
        };

        std::vector<cv::Scalar> colors = {
            cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255),
            cv::Scalar(255, 255, 0), cv::Scalar(255, 0, 255), cv::Scalar(0, 255, 255),
            cv::Scalar(128, 0, 0), cv::Scalar(0, 128, 0), cv::Scalar(0, 0, 128),
            cv::Scalar(128, 128, 0), cv::Scalar(128, 0, 128), cv::Scalar(0, 128, 128),
            cv::Scalar(64, 0, 0), cv::Scalar(0, 64, 0), cv::Scalar(0, 0, 64),
            cv::Scalar(64, 64, 0), cv::Scalar(64, 0, 64), cv::Scalar(0, 64, 64),
            cv::Scalar(192, 0, 0)
        };

        BYTETracker tracker(30, 50);  // 30

        // -------------------- 实时推理循环 --------------------
        while (true) {
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "无法获取摄像头帧" << std::endl;
                break;
            }

            // -------------------- 执行推理 --------------------
            trtyolo::Image image(frame.data, frame.cols, frame.rows);
            trtyolo::PoseRes result = detector->predict(image);

            std::vector<Object> objects = tools::revert2Tracker(result);

            std::vector<STrack> output = tracker.update(objects);

            cv::Mat res;

            tools::detect_By_tracker(frame, res, objects, output, SKELETON, KPS_COLORS, LIMB_COLORS);

            cv::imshow("Detection Result", res);





            // // -------------------- 绘制检测结果 --------------------
            // // 遍历每个检测到的目标
            // for (size_t i = 0; i < result.num; ++i) {
            //     auto& box = result.boxes[i];     // 当前目标的边界框
            //     int cls = result.classes[i];     // 当前目标的类别
            //     float score = result.scores[i];  // 当前目标的置信度
            //
            //     // 绘制边界框
            //     cv::rectangle(frame,
            //                 cv::Point(box.left, box.top),
            //                 cv::Point(box.right, box.bottom),
            //                 cv::Scalar(251, 81, 163), 2, cv::LINE_AA);
            //
            //     // 绘制标签和置信度
            //     std::string label_text = "Person " + std::to_string(score).substr(0, 5);
            //     int base_line;
            //     cv::Size label_size = cv::getTextSize(label_text, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &base_line);
            //     cv::rectangle(frame,
            //                 cv::Point(box.left, box.top),
            //                 cv::Point(box.left + label_size.width, box.top + label_size.height + base_line),
            //                 cv::Scalar(125, 40, 81), -1);
            //     cv::putText(frame, label_text,
            //                 cv::Point(box.left, box.top + label_size.height),
            //                 cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(253, 168, 208), 1);
            //
            //     // 绘制关键点和骨架
            //     int num_keypoints = result.kpts[i].size();
            //
            //     // 绘制关键点
            //     for (size_t j = 0; j < num_keypoints; ++j) {
            //         auto& kpt = result.kpts[i][j];
            //         if (kpt.conf.has_value() && kpt.conf.value() < 0.25) {
            //             continue;
            //         }
            //         if (int(kpt.x) % frame.cols != 0 && int(kpt.y) % frame.rows != 0) {
            //             cv::circle(frame, cv::Point(kpt.x, kpt.y), 5, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);
            //         }
            //     }
            //
            //     // 绘制骨架连接线
            //     bool is_pose = num_keypoints == 17;
            //     if (is_pose) {
            //         for (size_t j = 0; j < skeleton.size(); ++j) {
            //             const auto& sk = skeleton[j];
            //             const auto& kpt1 = result.kpts[i][sk[0] - 1];
            //             const auto& kpt2 = result.kpts[i][sk[1] - 1];
            //
            //             if (kpt1.conf < 0.25 || kpt2.conf < 0.25) {
            //                 continue;
            //             }
            //
            //             if (int(kpt1.x) % frame.cols == 0 || int(kpt1.y) % frame.rows == 0 || int(kpt1.x) < 0 || kpt1.y < 0) {
            //                 continue;
            //             }
            //             if (int(kpt2.x) % frame.cols == 0 || int(kpt2.y) % frame.rows == 0 || int(kpt2.x) < 0 || kpt2.y < 0) {
            //                 continue;
            //             }
            //
            //             cv::line(frame,
            //                     cv::Point(kpt1.x, kpt1.y),
            //                     cv::Point(kpt2.x, kpt2.y),
            //                     colors[j % colors.size()], 2, cv::LINE_AA);
            //         }
            //     }
            // }
            //
            // // 显示结果
            // cv::imshow("YOLO Pose Detection", frame);

            // 按 'q' 键退出
            if (cv::waitKey(1) == 'q') {
                break;
            }
        }

        // 释放资源
        cap.release();
        cv::destroyAllWindows();

    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}