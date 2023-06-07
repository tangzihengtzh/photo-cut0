#pragma once

// CLU.h

#include <opencv2\opencv.hpp> 
#include<vector>
extern "C" __declspec(dllexport) void ShowImage(const char* filepath, const char* save_path, const char* save_id);
void onMouse(int event, int x, int y, int flags, void* userdata);
/*
* 所需函数：
* 图像缩放展示、
* 返回鼠标点击处坐标
* 重采样函数
* 输出文件函数
*/

