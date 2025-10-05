# YOLO Anomaly Detection System - 异常行为检测系统
基于接入TensorRT的YOLOv11-Pose和BYTETrack多目标跟踪的实时摔倒检测系统

### [*@Valmorlee*](https://github.com/Valmorlee)

---

## 项目简介

本项目是一个基于深度学习的实时异常行为检测系统，专注于人体摔倒检测。系统使用YOLOv11姿态估计模型进行人体关键点检测，并结合BYTETrack多目标跟踪算法实现对多个目标的持续跟踪和状态判断。

### 核心功能

- 实时人体姿态估计（17个关键点）
- 多目标跟踪（BYTETrack算法）
- 摔倒检测算法（基于几何关系和关键点分析）
- 实时视频流处理
- FPS显示

## 技术架构

### 核心组件

1. **YOLOv11 Pose Estimation** - 用于人体关键点检测
2. **BYTETrack** - 多目标跟踪算法
3. **TensorRT** - 模型推理加速
4. **OpenCV** - 图像处理和显示

### 算法原理

摔倒检测基于以下5个判断条件：

1. 肩部最低点不低于脚踝中心点
2. 肩部最高点大于膝盖最低点
3. 关键点最小外接矩形宽高比
4. 膝盖与髋部连线与地面夹角
5. 肩、髋、膝夹角及髋、膝、踝夹角

## 项目结构

```
Yolo_AnomalyDetect/
├── CMakeLists.txt              # CMake构建配置
├── main.cpp                    # 主程序入口
├── include/                    # 头文件目录
│   ├── func.hpp               # 核心功能函数
│   ├── params.hpp             # 参数配置
│   ├── BYTETracker.h          # BYTETrack跟踪器
│   ├── STrack.h               # 跟踪对象定义
│   └── ...
├── src/                       # 源代码目录
│   ├── func.cpp               # 核心功能实现
│   ├── BYTETracker.cpp        # BYTETrack实现
│   ├── STrack.cpp             # 跟踪对象实现
│   └── ...
├── models/                    # 模型文件目录
│   ├── yolo11s-pose-v2.engine # TensorRT推理引擎
│   └── modelDescription.md    # 模型说明
├── out/                       # 可执行文件输出目录
└── shell.txt                  # 模型转换脚本
```

## 环境依赖

- CUDA 12.x
- cuDNN 8.9.7
- TensorRT 8.6.1.6
- OpenCV 4.x
- C++20标准
- Eigen3
- Python3 (开发环境)

## 安装与构建

### 1. 环境准备

确保系统已安装以下依赖：
```bash
# 安装OpenCV
sudo apt-get install libopencv-dev

# 安装CUDA和cuDNN (根据NVIDIA官方指南)

# 安装TensorRT (根据NVIDIA官方指南)
```

### 2. 构建项目

```bash
mkdir build
cd build
cmake ..
make
```

### 3. 模型准备

项目使用YOLOv11姿态估计模型，需要将PyTorch模型转换为TensorRT引擎：

```bash
# 导出ONNX模型
trtyolo export -w models/yolo11s-pose-v2.pt -v yolo11 -o models --iou_thres 0.5 --conf_thres 0.5 -b -1 -s

# 转换为TensorRT引擎
trtexec --onnx=models/yolo11s-pose-v2.onnx --saveEngine=models/yolo11s-pose-v2.engine --fp16 \
        --staticPlugins=/home/valmorx/ContestSource/TensorRT-YOLO/lib/libcustom_plugins.so \
        --setPluginsToSerialize=/home/valmorx/ContestSource/TensorRT-YOLO/lib/libcustom_plugins.so \
        --minShapes=images:1x3x640x640 \
        --optShapes=images:4x3x640x640 \
        --maxShapes=images:8x3x640x640
```

## 使用方法

### 运行程序

```bash
cd out
./Yolo_AnomalyDetect
```

程序会自动打开摄像头并开始实时检测。按 'q' 键退出程序。

### 参数配置

在 [params.hpp](include/params.hpp) 文件中可以调整以下参数：

- `cap_index`: 摄像头索引
- `cap_width/cap_height`: 视频分辨率
- `tracker_frameRate`: 跟踪器帧率
- 各种摔倒检测算法阈值参数

## 系统特点

### 实时性
- 基于TensorRT的模型推理加速
- 优化的图像处理流程
- 实时FPS显示

### 准确性
- 多条件综合判断摔倒状态
- 多目标持续跟踪
- 关键点置信度过滤

### 可扩展性
- 模块化设计，易于扩展新功能
- 清晰的代码结构和注释
- 易于调整的参数配置

## 版本信息

当前版本: v1.0.2

## 开发者信息

项目使用C++20标准开发，采用现代C++特性，包括：
- 智能指针
- 自动类型推导
- Lambda表达式
- 命名空间管理

## 注意事项

1. 确保模型路径在 [params.hpp](include/params.hpp) 中正确配置
2. 摄像头索引可能需要根据实际设备调整
3. 需要足够的GPU显存来运行TensorRT引擎
4. 摔倒检测准确性依赖于关键点检测精度

## 未来改进方向

1. 支持更多类型的异常行为检测
2. 增加报警机制（声音、邮件等）
3. 支持视频文件输入
4. 增加检测结果保存功能
5. 优化算法提高检测准确率