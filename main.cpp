#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

#include "BYTETracker.h"
#include "func.hpp"
#include "trtyolo.hpp"

int main() {

    // func::anomaly_detect();
    // func::anomaly_detect_multiBatch();
    func::anomaly_detect_video("../videoX.mp4", "../output_video.mp4", false);

}