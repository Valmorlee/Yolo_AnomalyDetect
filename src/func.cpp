//
// Created by valmorx on 2025/10/5.
//

#include "func.hpp"

namespace tools {

    // bool isTracking(int classId) {
    //     for (auto i: params::trackClass) {
    //         if (classId == i) return true;
    //     }
    //     return false;
    // }

    /**
     * @brief 获取以中心点为坐标的矩形
     * @param center
     * @param width
     * @param height
     * @return
     */
    cv::Rect get_centerRect(cv::Point2f center, float width, float height){
        float leftBound = center.x - width / 2.0 > 0 ? center.x - width / 2.0 : 0;
        float rightBound = center.x + width / 2.0 < params::cap_width ? center.x + width / 2.0 : params::cap_width;
        float upperBound = center.y - height / 2.0 > 0 ? center.y - height / 2.0 : 0;
        float lowerBound = center.y + height / 2.0 < params::cap_height ? center.y + height / 2.0 : params::cap_height;

        return cv::Rect(leftBound, upperBound, rightBound - leftBound, lowerBound - upperBound);
    }

    /**
     * @brief 筛选出检测结果中概率最大的结果
     * @param boxes 检测结果
     * @return 筛选结果
     */
    base::dataBox filterBoxes(std::vector<base::dataBox> &boxes) {
        if (boxes.empty()) return {};
        sort(boxes.begin(), boxes.end(), [](base::dataBox &a, base::dataBox &b) {return a.prob > b.prob;});
        return boxes[0];
    }

    /**
     * @brief 绘制检测结果
     * @param input 输入图像
     * @param output 绘制结果
     * @param color 绘制颜色
     */
    void drawRes(cv::Mat &input, base::dataBox &output, const Scalar &color) {
        cv::putText(input,
            cv::format("%d",output.classId),
            cv::Point2f(output.leftBound,output.topBound-5),
            0,0.6,color,2,LINE_AA);

        Rect2f preBox = cv::Rect2f(output.leftBound,output.topBound,output.width,output.height);
        circle(input,output.centerPoint,6,color,-1);
        cv::rectangle(input,
            preBox,
            color,1);
    }

    /**
     * @brief 绘制关键点、骨架与检测框
     * @param image 输入图像
     * @param res 绘制结果
     * @param objs 检测结果
     * @param SKELETON 骨架信息
     * @param KPS_COLORS 关键点颜色
     * @param LIMB_COLORS 骨架颜色
     */
    void draw_KPS_objects(
        const cv::Mat &image,
        cv::Mat &res,
        const std::vector<Object> &objs,
        const std::vector<std::vector<unsigned int>> &SKELETON,
        const std::vector<std::vector<unsigned int>> &KPS_COLORS,
        const std::vector<std::vector<unsigned int>> &LIMB_COLORS
    ) {
        res = image.clone();
        const int num_point = 17;  // COCO数据集关键点数
        for (auto &obj: objs) {
            // 绘制边界框
            cv::rectangle(
                    res,
                    obj.rect,
                    {0, 0, 255},  // 红色
                    2
            );

            // 绘制标签文本
            char text[256];
            sprintf(
                    text,
                    "person %.1f%%",
                    obj.prob * 100
            );

            int baseLine = 0;
            cv::Size label_size = cv::getTextSize(
                    text,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.4,
                    1,
                    &baseLine
            );

            int x = (int) obj.rect.x;
            int y = (int) obj.rect.y + 1;

            if (y > res.rows)
                y = res.rows;

            // 绘制标签背景
            cv::rectangle(
                    res,
                    cv::Rect(x, y, label_size.width, label_size.height + baseLine),
                    {0, 0, 255},
                    -1
            );

            // 绘制标签文字
            cv::putText(
                    res,
                    text,
                    cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.4,
                    {255, 255, 255},
                    1
            );

            // 绘制关键点和骨架
            auto &kps = obj.kps;
            for (int k = 0; k < num_point + 2; k++)
            {
                if (k < num_point)
                {
                    int kps_x = std::round(kps[k * 3]);
                    int kps_y = std::round(kps[k * 3 + 1]);
                    float kps_s = kps[k * 3 + 2];
                    if (kps_s > 0.5f)
                    {
                        // 绘制关键点
                        cv::Scalar kps_color = cv::Scalar(KPS_COLORS[k][0], KPS_COLORS[k][1], KPS_COLORS[k][2]);
                        cv::circle(res, {kps_x, kps_y}, 5, kps_color, -1);
                    }
                }
                
                // 绘制骨架连接线
                auto &ske = SKELETON[k];
                int pos1_x = std::round(kps[(ske[0] - 1) * 3]);
                int pos1_y = std::round(kps[(ske[0] - 1) * 3 + 1]);

                int pos2_x = std::round(kps[(ske[1] - 1) * 3]);
                int pos2_y = std::round(kps[(ske[1] - 1) * 3 + 1]);

                float pos1_s = kps[(ske[0] - 1) * 3 + 2];
                float pos2_s = kps[(ske[1] - 1) * 3 + 2];

                if (pos1_s > 0.5f && pos2_s > 0.5f)
                {
                    cv::Scalar limb_color = cv::Scalar(LIMB_COLORS[k][0], LIMB_COLORS[k][1], LIMB_COLORS[k][2]);
                    cv::line(res, {pos1_x, pos1_y}, {pos2_x, pos2_y}, limb_color, 2);
                }
            }
        }
    }

    std::vector<base::dataBox> revert2Box(const trtyolo::PoseRes &res) {
        std::vector<base::dataBox> boxes;
        float heightX = params::cap_height / 2.0 - params::roi_height / 2.0;
        float widthX = params::cap_width / 2.0 - params::roi_width / 2.0;
        for (int i = 0; i < res.boxes.size(); i++) {

            boxes.emplace_back(base::dataBox(res.boxes[i].left + widthX, res.boxes[i].right + widthX, res.boxes[i].top + heightX, res.boxes[i].bottom + heightX, res.scores[i], res.classes[i]));

        }

        return boxes;
    }

    std::vector<Object> revert2Tracker(const trtyolo::PoseRes &res) {
        std::vector<Object> objects;
        float heightX = params::cap_height / 2.0 - params::roi_height / 2.0;
        float widthX = params::cap_width / 2.0 - params::roi_width / 2.0;
        for (int i = 0; i < res.boxes.size(); i++) {
            Rect_<float> rect(res.boxes[i].left + widthX, res.boxes[i].top + heightX, 
                             res.boxes[i].right - res.boxes[i].left, res.boxes[i].bottom - res.boxes[i].top);
            
            Object tmp {rect, res.classes[i], res.scores[i]};
            
            // 填充关键点信息
            tmp.kps.reserve(res.kpts[i].size() * 3);
            for (const auto& kpt : res.kpts[i]) {
                tmp.kps.push_back(kpt.x + widthX);     // x坐标
                tmp.kps.push_back(kpt.y + heightX);    // y坐标
                tmp.kps.push_back(kpt.conf.value());          // 置信度
            }
            
            objects.push_back(tmp);
        }
        return objects;
    }

    std::vector<base::dataBox> tracker2Box(vector<STrack> &output) {
        std::vector<base::dataBox> boxes;
        for (int i = 0 ; i < output.size(); i++) {
            std::vector<float> _tlwh = output[i].tlwh;
            if (_tlwh[2]*_tlwh[3] > 20) {
                boxes.emplace_back(
                    base::dataBox(
                        Point2f(_tlwh[0],_tlwh[1]),
                        _tlwh[2],
                        _tlwh[3],
                        output[i].score,
                        output[i].track_id
                    )
                );
            }
        }
        return boxes;
    }

    /**
 * 摔倒检测算法，基于人体关键点位置和几何关系判断是否发生摔倒
 * @param kps 关键点坐标数组 [x1,y1,s1,x2,y2,s2,...]
 * @return true表示检测到摔倒，false表示正常状态
 */
bool fallDetXAlgorithm(const std::vector<float>& kps)
{
    // 设置一个判断是否为摔倒的变量
    bool is_fall = false;

    // 1. 获取用于判断的关键点坐标
    cv::Point L_shoulder = cv::Point((int)kps[5 * 3], (int)kps[5 * 3 + 1]);  // 左肩
    float L_shoulder_confi = kps[5 * 3 + 2];
    cv::Point R_shoulder = cv::Point((int)kps[6 * 3], (int)kps[6 * 3 + 1]);  // 右肩
    float R_shoulder_confi = kps[6 * 3 + 2];
    cv::Point C_shoulder = cv::Point((int)(L_shoulder.x + R_shoulder.x) / 2, (int)(L_shoulder.y + R_shoulder.y) / 2);  // 肩部中点

    cv::Point L_hip = cv::Point((int)kps[11 * 3], (int)kps[11 * 3 + 1]);  // 左髋
    float L_hip_confi = kps[11 * 3 + 2];
    cv::Point R_hip = cv::Point((int)kps[12 * 3], (int)kps[12 * 3 + 1]);  // 右髋
    float R_hip_confi = kps[12 * 3 + 2];
    cv::Point C_hip = cv::Point((int)(L_hip.x + R_hip.x) / 2, (int)(L_hip.y + R_hip.y) / 2);  // 髋部中点

    cv::Point L_knee = cv::Point((int)kps[13 * 3], (int)kps[13 * 3 + 1]);  // 左膝
    float L_knee_confi = kps[13 * 3 + 2];
    cv::Point R_knee = cv::Point((int)kps[14 * 3], (int)kps[14 * 3 + 1]);  // 右膝
    float R_knee_confi = kps[14 * 3 + 2];
    cv::Point C_knee = cv::Point((int)(L_knee.x + R_knee.x) / 2, (int)(L_knee.y + R_knee.y) / 2);  // 膝部中点

    cv::Point L_ankle = cv::Point((int)kps[15 * 3], (int)kps[15 * 3 + 1]);  // 左踝
    float L_ankle_confi = kps[15 * 3 + 2];
    cv::Point R_ankle = cv::Point((int)kps[16 * 3], (int)kps[16 * 3 + 1]);  // 右踝
    float R_ankle_confi = kps[16 * 3 + 2];
    cv::Point C_ankle = cv::Point((int)(L_ankle.x + R_ankle.x) / 2, (int)(L_ankle.y + R_ankle.y) / 2);  // 计算脚踝中点

    // 2. 第一个判定条件：若肩的纵坐标最小值min(L_shoulder.y, R_shoulder.y)不低于脚踝的中心点的纵坐标C_ankle.y
    // 且关键点置信度大于预设的阈值，则疑似摔倒。
    if (L_shoulder_confi > params::KPS_threshold && R_shoulder_confi > params::KPS_threshold && L_ankle_confi > params::KPS_threshold && R_ankle_confi > params::KPS_threshold)
    {
        int shoulder_y_min = std::min(L_shoulder.y, R_shoulder.y);
        if (shoulder_y_min >= C_ankle.y)
        {
            is_fall = true;
            return is_fall;
        }
    }

    // 3. 第二个判断条件：若肩的纵坐标最大值max(L_shoulder.y, R_shoulder.y)大于膝盖纵坐标的最小值min(L_knee.y, R_knee.y)，
    // 且关键点置信度大于预设的阈值，则疑似摔倒。
    if (L_shoulder_confi > params::KPS_threshold && R_shoulder_confi > params::KPS_threshold && L_knee_confi > params::KPS_threshold && R_knee_confi > params::KPS_threshold)
    {
        int shoulder_y_max = std::max(L_shoulder.y, R_shoulder.y);
        int knee_y_min = std::min(L_knee.y, R_knee.y);
        if (shoulder_y_max > knee_y_min)
        {
            is_fall = true;
            return is_fall;
        }
    }

    // 4. 第三个判断条件：计算关键点最小外接矩形的宽高比。
    // 若(xmax-xmin) / (ymax-ymin)大于指定的比例阈值，则判定为摔倒。
    const int num_point = params::KPS_num;  // 17个关键点

    // 初始化xmin, ymin为最大值，xmax, ymax为最小值
    int xmin = std::numeric_limits<int>::max();
    int ymin = std::numeric_limits<int>::max();
    int xmax = std::numeric_limits<int>::min();
    int ymax = std::numeric_limits<int>::min();

    for (int k = 0; k < num_point + 2; k++)
    {
        if (k < num_point)
        {
            int kps_x = std::round(kps[k * 3]);  // 关键点x
            int kps_y = std::round(kps[k * 3 + 1]);  // 关键点y
            float kps_s = kps[k * 3 + 2];  // 可见性

            if (kps_s > 0.0f)
            {
                // 更新xmin, xmax, ymin, ymax
                xmin = std::min(xmin, kps_x);
                xmax = std::max(xmax, kps_x);
                ymin = std::min(ymin, kps_y);
                ymax = std::max(ymax, kps_y);
            }
        }
    }

    // 检查是否存在有效的宽度和高度
    if (xmax > xmin && ymax > ymin)
    {
        float aspect_ratio = static_cast<float>(xmax - xmin) / (ymax - ymin);

        // 如果宽高比大于指定阈值，则判定为摔倒
        if (aspect_ratio > params::Rect_WidthHeight_Ratio)
        {
            is_fall = true;
            return is_fall;
        }
    }

    // 5. 第四个判断条件：通过两膝与髋部中心点的连线与地面的夹角判断。
    // 若min(θ1 ,θ2 )＜th1 或 max(θ1 ,θ2 )＜th2，且关键点置信度大于预设的阈值，则疑似摔倒
    if (L_knee_confi > params::KPS_threshold && R_knee_confi > params::KPS_threshold && L_hip_confi > params::KPS_threshold && R_hip_confi > params::KPS_threshold)
    {
        // 左膝与髋部中心的角度
        float theta1 = std::atan2(L_knee.y - C_hip.y, L_knee.x - C_hip.x) * 180.0f / CV_PI;
        // 右膝与髋部中心的角度
        float theta2 = std::atan2(R_knee.y - C_hip.y, R_knee.x - C_hip.x) * 180.0f / CV_PI;

        float min_theta = std::min(std::abs(theta1), std::abs(theta2));
        float max_theta = std::max(std::abs(theta1), std::abs(theta2));

        // 设定阈值 th1 和 th2，用于判定是否摔倒
        float th1 = params::MIN_KneeHip_Theta;  // 最小角度阈值
        float th2 = params::MAX_KneeHip_Theta;  // 最大角度阈值

        if ((min_theta) < th1 && (max_theta < th2))
        {
            is_fall = true;
            return is_fall;
        }
    }

    // 6. 第五个判断条件：通过肩、髋部、膝盖夹角，髋部、膝盖、脚踝夹角判断。
    // 如果所有关键点的置信度都大于阈值，我们继续进行角度的计算。
    if (L_shoulder_confi > params::KPS_threshold && R_shoulder_confi > params::KPS_threshold && L_hip_confi > params::KPS_threshold && R_hip_confi > params::KPS_threshold &&
        L_knee_confi > params::KPS_threshold && R_knee_confi > params::KPS_threshold && L_ankle_confi > params::KPS_threshold && R_ankle_confi > params::KPS_threshold)
    {
        // 计算向量 v1 和 v2
        cv::Point2f v1(C_shoulder.x - C_hip.x, C_shoulder.y - C_hip.y);
        cv::Point2f v2(C_knee.x - C_hip.x, C_knee.y - C_hip.y);

        // 计算向量 v3 和 v4
        cv::Point2f v3(C_hip.x - C_knee.x, C_hip.y - C_knee.y);
        cv::Point2f v4(C_ankle.x - C_knee.x, C_ankle.y - C_knee.y);

        // 计算向量 v1 和 v2 的夹角 θ3
        float dot_product1 = v1.x * v2.x + v1.y * v2.y;
        float magnitude1 = std::sqrt(v1.x * v1.x + v1.y * v1.y) * std::sqrt(v2.x * v2.x + v2.y * v2.y);
        float theta3 = std::acos(dot_product1 / magnitude1) * 180.0f / CV_PI;

        // 计算向量 v3 和 v4 的夹角 θ4
        float dot_product2 = v3.x * v4.x + v3.y * v4.y;
        float magnitude2 = std::sqrt(v3.x * v3.x + v3.y * v3.y) * std::sqrt(v4.x * v4.x + v4.y * v4.y);
        float theta4 = std::acos(dot_product2 / magnitude2) * 180.0f / CV_PI;

        // 判断是否符合摔倒条件
        if ((theta3 < params::ShoulderBackKnee_Theta) && (theta4 < params::BackKneeFoot_Theta))
        {
            is_fall = true;
        }

        return is_fall;
    }

    return is_fall;
}

    /**
 * 绘制检测结果（带跟踪功能）
 * @param image 原始图像
 * @param res 绘制结果图像（输出参数）
 * @param objs 检测到的对象列表
 * @param output_stracks 跟踪轨迹列表
 * @param SKELETON 骨架连接关系
 * @param KPS_COLORS 关键点颜色
 * @param LIMB_COLORS 肢体连接颜色
 */
void detect_By_tracker(
        const cv::Mat &image,
        cv::Mat &res,
        const std::vector<Object> &objs,
        const std::vector<STrack>& output_stracks,
        const std::vector<std::vector<unsigned int>> &SKELETON,
        const std::vector<std::vector<unsigned int>> &KPS_COLORS,
        const std::vector<std::vector<unsigned int>> &LIMB_COLORS
)
{
    res = image.clone();
    const int num_point = 17;  // COCO数据集关键点数
    for (auto &obj: objs)
    {
        auto &kps = obj.kps;

        // 绘制关键点和骨架
        for (int k = 0; k < num_point + 2; k++)
        {
            if (k < num_point)
            {
                int kps_x = std::round(kps[k * 3]);
                int kps_y = std::round(kps[k * 3 + 1]);
                float kps_s = kps[k * 3 + 2];

                if (kps_s > 0.0f)
                {
                    cv::Scalar kps_color = cv::Scalar(KPS_COLORS[k][0], KPS_COLORS[k][1], KPS_COLORS[k][2]);
                    cv::circle(res, {kps_x, kps_y}, 5, kps_color, -1);
                }
            }

            // 绘制骨架连接线
            auto &ske = SKELETON[k];
            int pos1_x = std::round(kps[(ske[0] - 1) * 3]);
            int pos1_y = std::round(kps[(ske[0] - 1) * 3 + 1]);

            int pos2_x = std::round(kps[(ske[1] - 1) * 3]);
            int pos2_y = std::round(kps[(ske[1] - 1) * 3 + 1]);

            float pos1_s = kps[(ske[0] - 1) * 3 + 2];
            float pos2_s = kps[(ske[1] - 1) * 3 + 2];

            if (pos1_s > 0.0f && pos2_s > 0.0f)
            {
                cv::Scalar limb_color = cv::Scalar(LIMB_COLORS[k][0], LIMB_COLORS[k][1], LIMB_COLORS[k][2]);
                cv::line(res, {pos1_x, pos1_y}, {pos2_x, pos2_y}, limb_color, 2);
            }
        }

        // 摔倒检测
        bool is_fall = fallDetXAlgorithm(kps);

        // 绘制状态标签
        char text_[32];
        if (is_fall)
        {
            sprintf(text_, "STATUS:FALL");
        }
        else
        {
            sprintf(text_, "STATUS:NORMAL");
        }

        int baseLine_ = 0;
        cv::Size label_size_ = cv::getTextSize(
                text_,
                cv::FONT_HERSHEY_SIMPLEX,
                0.4,
                1,
                &baseLine_
        );

        int x_ = (int) obj.rect.x;
        int y_ = (int) obj.rect.y - 15;

        if (y_ > res.rows)
            y_ = res.rows;

        // 根据状态绘制不同颜色的标签
        if (is_fall)
        {
            cv::rectangle(
                res,
                cv::Rect(x_, y_, label_size_.width, label_size_.height + baseLine_),
                {0, 0, 255},  // 红色表示摔倒
                -1
            );
        }
        else
        {
            cv::rectangle(
                res,
                cv::Rect(x_, y_, label_size_.width, label_size_.height + baseLine_),
                {0, 255, 0},  // 绿色表示正常
                -1
            );
        }

        cv::putText(
                res,
                text_,
                cv::Point(x_, y_ + label_size_.height),
                cv::FONT_HERSHEY_SIMPLEX,
                0.4,
                {0, 0, 0},
                1
        );
    }

    // 绘制跟踪轨迹
    for (int i = 0; i < output_stracks.size(); i++)
    {
        std::vector<float> tlwh = output_stracks[i].tlwh;
        // bool vertical = tlwh[2] / tlwh[3] > 1.6;
        // if (tlwh[2] * tlwh[3] > 20 && !vertical)
        if (tlwh[2] * tlwh[3] > 20)
        {
            cv::Scalar s = random_get_color(output_stracks[i].track_id);

            char text[256];
            sprintf(
                    text,
                    "ID: %d",
                    output_stracks[i].track_id
            );

            int baseLine = 0;
            cv::Size label_size = cv::getTextSize(
                    text,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.4,
                    1,
                    &baseLine
            );

            int x = (int) tlwh[0];
            int y = (int) tlwh[1] + 1;

            if (y > res.rows)
                y = res.rows;

            // 绘制ID标签背景
            cv::rectangle(
                res,
                cv::Rect(x, y, label_size.width, label_size.height + baseLine),
                s,
                cv::FILLED
            );

            // 绘制边界框
            cv::rectangle(
                res,
                cv::Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3] + baseLine),
                s,
                2
            );

            // 绘制ID文本
            cv::putText(
                res,
                text,
                cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX,
                0.4,
                {255, 255, 255},
                1
            );
        }
    }
}

    /**
 * 绘制FPS信息
 * @param image 图像
 * @return 0表示成功
 */
    int fps_display(cv::Mat& image)
    {
        // 计算移动平均FPS
        float avg_fps = 0.f;
        {
            static int64 t0 = 0;
            static float fps_history[10] = { 0.f };

            // 获取实时时间
            int64 t1 = cv::getTickCount();
            if (t0 == 0)
            {
                t0 = t1;
                return 0;
            }

            // 计算FPS (注意: getTickCount()返回的是时钟周期数，需要除以getTickFrequency()得到秒)
            float fps = cv::getTickFrequency() / (t1 - t0);
            t0 = t1;

            // 更新FPS历史记录
            for (int i = 9; i >= 1; i--)
            {
                fps_history[i] = fps_history[i - 1];
            }
            fps_history[0] = fps;

            if (fps_history[9] == 0.f)
            {
                return 0;
            }

            // 计算平均FPS
            for (int i = 0; i < 10; i++)
            {
                avg_fps += fps_history[i];
            }
            avg_fps /= 10.f;
        }

        // 绘制FPS信息
        char text[32];
        sprintf(text, "FPS: %.2f", avg_fps);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        // 右上角位置
        int x = image.cols - label_size.width - 10;  // 右边距10像素
        int y = label_size.height + 10;              // 上边距10像素

        // 绘制FPS标签背景
        cv::rectangle(image, cv::Rect(cv::Point(x, y - label_size.height), cv::Size(label_size.width, label_size.height + baseLine)),
            cv::Scalar(0, 0, 0), -1);

        // 绘制FPS文本
        cv::putText(image, text, cv::Point(x, y),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

        return 0;
    }

}

namespace func {

    /**
 * 摔倒检测 主程序
 * @return 0表示成功
 */
    int anomaly_detect() {

        try {
            trtyolo::InferOption option;
            option.enableSwapRB();  // BGR->RGB转换

            std::cout << "Init Basic Option! Ciallo～ (∠・ω< )⌒★" << std::endl;

            auto detector = std::make_unique<trtyolo::PoseModel>(
                params::Engine_Path,  // 模型路径
                option                  // 推理设置
            );

            std::cout << "Init Engine! Ciallo～ (∠・ω< )⌒★" << std::endl;

            cv::VideoCapture cap(params::cap_index, cv::CAP_V4L2);
            if (!cap.isOpened()) {
                throw std::runtime_error("摄像头无法初始化");
            }

            cap.set(cv::CAP_PROP_FRAME_WIDTH, params::cap_width);
            cap.set(cv::CAP_PROP_FRAME_HEIGHT, params::cap_height);
            cap.set(cv::CAP_PROP_FPS, params::tracker_frameRate);

            std::cout << "Init Camera! Ciallo～ (∠・ω< )⌒★" << std::endl;

            BYTETracker tracker(params::tracker_frameRate, params::tracker_bufferSize);
            cv::Mat frame;

            std::cout << "AnomalyDetect " << params::version << " Init Done! Ciallo～ (∠・ω< )⌒★" << std::endl;

            while (true) {
                cap >> frame;
                if (frame.empty()) {
                    std::cerr << "无法获取摄像头帧" << std::endl;
                    break;
                }

                trtyolo::Image image(frame.data, frame.cols, frame.rows);
                trtyolo::PoseRes result = detector->predict(image);

                std::vector<Object> objects = tools::revert2Tracker(result);

                std::vector<STrack> output = tracker.update(objects);

                cv::Mat res;

                tools::detect_By_tracker(frame, res, objects, output, params::SKELETON, params::KPS_COLORS, params::LIMB_COLORS);

                tools::fps_display(res);

                cv::imshow("Detection Result", res);

                // 按 'q' 键退出
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }

            // 释放资源
            cap.release();
            cv::destroyAllWindows();

        } catch (const std::exception& e) {
            std::cerr << "程序异常错误: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "主程序已关闭" << std::endl;
        return EXIT_SUCCESS;
    }

}