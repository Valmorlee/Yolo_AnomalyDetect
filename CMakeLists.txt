cmake_minimum_required(VERSION 3.26)
project(Yolo_AnomalyDetect VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 20)

set(project_root ${PROJECT_SOURCE_DIR})
set(project_src ${project_root}/src)
set(project_include ${project_root}/include)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)

# 旧协议指令屏蔽
if(POLICY CMP0146)
    cmake_policy(SET CMP0146 OLD)
endif()

#========== CUDA ==========
set(CMAKE_CUDA_ARCHITECTURES 87)
set(CMAKE_CUDA_COMPILER "/usr/local/cuda/bin/nvcc")
find_package(CUDA REQUIRED)

include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)

include_directories(/home/valmorx/DeepLearningSource/cudnn-8.9.7/include) # jetson del
link_directories(/home/valmorx/DeepLearningSource/cudnn-8.9.7/lib) # jetson del

#========== OpenCV ==========
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

aux_source_directory(${PROJECT_BINARY_DIR}/../src SRC)
include_directories(${PROJECT_BINARY_DIR}/../include)
link_directories(${PROJECT_BINARY_DIR}/../lib/)

#========= TENSORRT =========

include_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/include) # jetson del
link_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/lib) # jetson del

#========== EXTRA ==========

set(RT_YOLO_ROOT /home/valmorx/ContestSource/TensorRT-YOLO)
include_directories(${RT_YOLO_ROOT}/include)
link_directories(${RT_YOLO_ROOT}/lib)

find_package(Python3 COMPONENTS Interpreter Development)
include_directories(${Python3_INCLUDE_DIRS})

find_package(CURL REQUIRED)

#========= Eigen3 ===========
include_directories(/usr/include/eigen3)

include_directories(
        ${project_include}
)

add_executable(Yolo_AnomalyDetect
        main.cpp
        ${SRC}
        include/params.hpp
        include/func.hpp

)

target_link_libraries(Yolo_AnomalyDetect
        ${OpenCV_LIBS}
        ${Python3_LIBRARIES}
        ${RT_YOLO_ROOT}/lib/libtrtyolo.so
        /usr/local/cuda/lib64/libcudart.so.12

)

