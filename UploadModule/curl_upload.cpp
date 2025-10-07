#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

std::string getCurrentTimeStringWithMs() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

/**
 * @brief Simple function to upload a file using libcurl
 * 
 * @param url The URL to upload to
 * @param filepath Path to the file to upload
 * @return true if successful, false otherwise
 */
bool uploadFile(const std::string& url, const std::string& filepath) {
    CURL *curl;
    CURLcode res;
    bool success = false;

    struct curl_slist *headers = NULL;
    
    // Initialize curl
    curl = curl_easy_init();
    if(curl) {
        // Set the URL for the POST
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // Create form data
        curl_mime *form = NULL;
        curl_mimepart *field = NULL;
        form = curl_mime_init(curl);
        
        // Add file to the form
        field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, filepath.c_str());
        
        // Set the form as the POST data
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            success = true;
        }
        
        // Cleanup
        curl_mime_free(form);
        curl_easy_cleanup(curl);
    }
    
    return success;
}

/**
 * @brief Function to upload JSON data using libcurl
 * 
 * @param url The URL to upload to
 * @param jsonData JSON data to upload
 * @return true if successful, false otherwise
 */
bool uploadJson(const std::string& url, const json& jsonData) {
    CURL *curl;
    CURLcode res;
    bool success = false;
    
    // Convert JSON to string
    std::string jsonString = jsonData.dump();
    
    // Initialize curl
    curl = curl_easy_init();
    if(curl) {
        // Set the URL for the POST
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // Set headers for JSON
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Set POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            success = true;
        }
        
        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return success;
}

// Callback function to handle the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t realsize = size * nmemb;
    response->append((char*)contents, realsize);
    return realsize;
}

int main() {

    const std::string status_file = "/home/jetson/CLionProjects/Yolo_AnomalyDetect/UploadModule/upload_tmpfile.txt";
    const std::string url = "http://fc-mp-860818b9-ec1a-42d4-a680-7fea0bf26c95.next.bspapp.com/tt";
    
    // Global curl initialization
    curl_global_init(CURL_GLOBAL_ALL);

    std::cout << "Init Curl Connection!" << std::endl;
    
    while (true) {
        // 读取状态文件
        std::ifstream file(status_file);
        if (!file.is_open()) {
            std::this_thread::sleep_for(std::chrono::seconds(0));
            std::cout << "Waiting for status file..." << std::endl;
            continue;
        }
        
        int status;
        file >> status;
        file.close();
        
        if (status == 1) {

            std::cout << "Anomaly detected!" << std::endl;

            // Create JSON payload
            nlohmann::json payload = {
                {"openid", "15657137860"},
                {"state", 1},
                {"time", getCurrentTimeString()}
            };

            CURL* curl = curl_easy_init();
            if (curl) {
                CURLcode res;
                struct curl_slist* headers = nullptr;

                // 设置请求头为application/json，与Python版本保持一致
                headers = curl_slist_append(headers, "Content-Type: application/json");

                curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,false);//设定为不验证证书和HOST
                curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,false);

                // 设置curl选项
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

                // 设置为POST请求
                curl_easy_setopt(curl, CURLOPT_POST, 1L);

                // 将JSON数据转换为字符串并设置为请求体
                std::string json_str = payload.dump();
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_str.length());

                // 设置响应回调
                std::string response_string;
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

                // 执行请求
                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                else
                    std::cout << "Response: " << response_string << std::endl;

                // 清理资源
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
            }
            
            // 重置状态文件为0
            // std::ofstream reset_file(status_file);
            // if (reset_file.is_open()) {
            //     reset_file << "0";
            //     reset_file.close();
            // }
        } else if (status == 0) {
            // 继续循环
            std::cout << "No anomaly detected." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(0));
            continue;
        } else {
            // 无效状态值
            std::cerr << "Invalid status value: " << status << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(0));
        }
    }
    
    // Global curl cleanup
    curl_global_cleanup();
    
    return 0;
}