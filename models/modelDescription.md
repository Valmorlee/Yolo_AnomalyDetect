# Yolo-AnomalyDetect Engine 模型版本说明日志

## yolo11s-pose-v2 (Updated On 2025.10.5)

- **文件**:
  - PyTorch模型: yolo11s-pose-v2.pt
  - ONNX模型: yolo11s-pose-v2.onnx
  - TensorRT引擎: yolo11s-pose-v2.engine
- **文件大小**:
  - PT: 19,895.7 KB
  - ONNX: 38,808.3 KB
  - Engine: 41,811.4 KB
- **特点**:
  - 加入了Dynamic Batch【动态批处理】的支持

## yolo11s-pose (Origin On 2025.10.5)

- **文件**: 
  - PyTorch模型: yolo11s-pose.pt
  - ONNX模型: yolo11s-pose.onnx
  - TensorRT引擎: yolo11s-pose.engine
- **文件大小**: 
  - PT: 19,895.7 KB
  - ONNX: 39,077.2 KB
  - Engine: 41,041.5 KB
- **特点**: 
  - 基于YOLOv11的轻量级姿态估计模型
  - 支持人体17个关键点检测
  - 适用于通用场景的人体姿态估计任务
  - 模型结构相对简单，易于部署

## 使用建议

- **一般用途**: 如果是通用的人体姿态检测任务，两个版本都可以使用
- **精度优先**: 如果对关键点检测精度有更高要求，建议使用v2版本
- **性能优先**: 如果对推理速度有较高要求，建议使用v2版本
- **部署环境**: 两个版本的TensorRT引擎大小相近，部署时可根据精度需求选择