cmake_minimum_required(VERSION 3.26)

# Define the upload module
project(UploadModule)

# Find required packages
find_package(CURL REQUIRED)

# Add executable for upload module
add_executable(curl_upload curl_upload.cpp)

# Link libraries
target_link_libraries(curl_upload 
    ${CURL_LIBRARIES}
)

# Include directories
target_include_directories(curl_upload PRIVATE 
    ${CURL_INCLUDE_DIRS}
)

# Compile definitions
target_compile_definitions(curl_upload PRIVATE 
    ${JSON_CFLAGS_OTHER}
)