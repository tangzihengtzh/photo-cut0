#include"CLU.h"
#include<vector>
#include<thread>
#include<math.h>
#include<string>

using namespace std;
using namespace cv;

int gx, gy;
int clicked = 0;
cv::Mat srcImage;                   // 原始图像
cv::Mat dstImage;                   // 调整比例后的图像
std::vector<int> point_4_2 = {
    0,0,
    0,0,
    0,0,
    0,0
};//(X,Y)
int click_n = 0;

double res;
string save_path_e; 
string save_id_e;

__declspec(dllexport) void ShowImage(const char* filepath,const char* save_path,const char* save_id) {
    srcImage = cv::imread(filepath);  // 读取原始图像
    save_path_e = string(save_path);
    save_id_e = string(save_id);
    int targetWidth = 900;                         // 目标宽度
                                                   // 比例
    res = srcImage.cols / 900.0;
    int targetHeight = srcImage.rows / res;                        // 目标高度
    std::cout <<"source image width:"<<srcImage.cols << ",resized image width:900,scale ratio:" << res;

    cv::resize(srcImage, dstImage, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_LINEAR);

    cv::imshow("Image Viewer", dstImage);

    cv::setMouseCallback("Image Viewer", onMouse, nullptr);

    cv::waitKey(0);
    cv::destroyAllWindows();
}

void onMouse(int event, int x, int y, int flags, void* userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)   // 如果鼠标左键被按下
    {
        std::cout << "Left button of the mouse is clicked at position (" << x << ", " << y << ")" << std::endl;
        gx = x;
        gy = y;
        clicked = 1;

        // 在图像上绘制一个点
        cv::circle(dstImage, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1);

        // 更新窗口显示
        cv::imshow("Image Viewer", dstImage);

        point_4_2[2*click_n] = x;
        point_4_2[2*click_n + 1] = y;
        click_n += 1;
        if (click_n == 4)
        {
            std::cout << "click done\n";
            clicked = 0;
            for (int i = 0; i < 4; i++)
            {
                //std::cout << point_4_2[i] << "," << point_4_2[i] << std::endl;
                std::cout << "starting transform\n";

                //int cut_X = abs(point_4_2[4] - point_4_2[2]);
                //int cut_Y = abs(point_4_2[3] - point_4_2[1]);
                
                int cut_X = sqrt(
                    (point_4_2[4] - point_4_2[2]) * (point_4_2[4] - point_4_2[2])
                    +
                    (point_4_2[5] - point_4_2[3]) * (point_4_2[5] - point_4_2[3])
                );
                int cut_Y = sqrt(
                    (point_4_2[3] - point_4_2[1]) * (point_4_2[3] - point_4_2[1])
                    +
                    (point_4_2[2] - point_4_2[0]) * (point_4_2[2] - point_4_2[0])
                );

                //int src_X = int(cut_X);
                //int src_Y = int(cut_Y);

                
                int src_X = int(cut_X* res);
                int src_Y = int(cut_Y* res);

                double mx[] = {
                    point_4_2[2] - point_4_2[0],point_4_2[3] - point_4_2[1],
                    point_4_2[6] - point_4_2[0],point_4_2[7] - point_4_2[1],
                };
                std::cout << mx[0] << " " << mx[1] << "\n";
                std::cout << mx[2] << " " << mx[3] << "\n";
                //for (int i = 0; i < 4; i++)
                //{
                //    mx[i] *= res;
                //}

                cv::arrowedLine(dstImage, cv::Point(point_4_2[0], point_4_2[1]), cv::Point(point_4_2[0]+int(mx[0]), point_4_2[1]+int(mx[1])),
                    cv::Scalar(0, 0, 255), 2, cv::LINE_AA, 0, 0.1);
                cv::imshow("Image Viewer", dstImage);
                cv::arrowedLine(dstImage, cv::Point(point_4_2[0], point_4_2[1]), cv::Point(point_4_2[0]+int(mx[2]), point_4_2[1]+int(mx[3])),
                    cv::Scalar(0, 0, 255), 2, cv::LINE_AA, 0, 0.1);
                cv::imshow("Image Viewer", dstImage);
                //mx[0] = mx[0] / sqrt(mx[0] * mx[0] + mx[1] * mx[1]);
                //mx[1] = mx[1] / sqrt(mx[0] * mx[0] + mx[1] * mx[1]);
                //mx[2] = mx[2] / sqrt(mx[2] * mx[2] + mx[3] * mx[3]);
                //mx[3] = mx[3] / sqrt(mx[2] * mx[2] + mx[3] * mx[3]);
                //向量矩阵(第一行Y)

                std::cout << "size:" << src_Y << "," << src_X << std::endl;
                
                cv::Mat canvas(cv::Size(src_X, src_Y), CV_8UC3, cv::Scalar(0, 0, 0));

                std::vector<cv::Point2f> srcPoints{
                    { float(point_4_2[0] * res), float(point_4_2[1] * res) },   // 左上角
                    { float(point_4_2[6] * res), float(point_4_2[7] * res) },   // 右上角
                    { float(point_4_2[4] * res), float(point_4_2[5] * res) },   // 右下角
                    { float(point_4_2[2] * res), float(point_4_2[3] * res) }    // 左下角
                };

                // 目标图像上的四个点，依次对应 srcPoints 中的四个点
                std::vector<cv::Point2f> dstPoints{
                    {0, 0},       // 左上角
                    { float(src_X), 0 },     // 右上角
                    { float(src_X), float(src_Y) },   // 右下角
                    { 0, float(src_Y) }      // 左下角
                };

                cv::Mat M = cv::getPerspectiveTransform(srcPoints, dstPoints);

                cv::warpPerspective(srcImage, canvas, M, cv::Size(src_X, src_Y));

                //for (int i = 0; i < src_X; i++)
                //{
                //    for (int j = 0; j < src_Y; j++)
                //    {
                //        //canvas.at<cv::Vec3b>(j, i) = dstImage.at<cv::Vec3b>(int(point_4_2[1])+j, int(point_4_2[0]) +i);
                //        //canvas.at<cv::Vec3b>(j, i) = dstImage.at<cv::Vec3b>(j,i);
                //        //int sy = int(point_4_2[1] + (double(j) / src_Y) * src_Y);
                //        //int sx = int(point_4_2[0] + (double(i) / src_X) * src_X);
                //        double sy = point_4_2[1] * res;
                //        double sx = point_4_2[0] * res;
                //        sy += mx[1] * res * (double(j) / src_Y);
                //        sx += mx[0] * res * (double(j) / src_X);

                //        sy += mx[3] * res * (double(i) / src_Y);
                //        sx += mx[2] * res * (double(i) / src_X);
                //        canvas.at<cv::Vec3b>(j, i) = srcImage.at<cv::Vec3b>(int(sy),int(sx));
                //        if (i%20==0 && j%20==0)
                //        {
                //            cv::circle(dstImage, cv::Point(int(sx / res), int(sy / res)), 1, cv::Scalar(80, 100, 60), -1);
                //            cv::imshow("Image Viewer", dstImage);
                //        }
                //        //std::cout << sy << " " << sx << std::endl;
                //        //canvas.at<cv::Vec3b>(j, i) = srcImage.at<cv::Vec3b>(int(point_4_2[1]* res) + j, int(point_4_2[0]* res) + i);
                //        //std::cout << j << "," << i << "<<" << int(point_4_2[1] * 6.5) + j << ","<<int(point_4_2[0] * 6.5) + i << std::endl;
                //    }
                //}

                string save_path_name = save_path_e + save_id_e + string(".jpg");
                cv::imwrite(save_path_name, canvas);
                //cv::imwrite("out.jpg", canvas);

                // 显示这张空画布
                cv::imshow("Canvas", canvas);
                
                cv::destroyAllWindows();
                click_n = 0;

                // 等待按下任意按键后关闭窗口
                //cv::waitKey(0);

                //// 定义原始四边形的四个点
                //Point2f src[4] = { 
                //    Point2f(point_4_2[0], point_4_2[1]),
                //    Point2f(point_4_2[2], point_4_2[3]),
                //    Point2f(point_4_2[4], point_4_2[5]),
                //    Point2f(point_4_2[6], point_4_2[7])
                //};

                //

                //// 定义目标矩形的四个点
                //Point2f dst[4] = { 
                //    Point2f(0, 0),
                //    Point2f(cut_L, 0),
                //    Point2f(cut_L, cut_H),
                //    Point2f(0, cut_H)
                //};
                //// 计算变换矩阵
                //Mat M = getPerspectiveTransform(src, dst);

                //Mat warped;
                //warpPerspective(dstImage, warped, M, Size(500, 800));

                //// 显示结果
                //imshow("Warped Image", warped);
                //waitKey(0);
                
            }
            //cv::destroyAllWindows();
            return;
        }


    }
}

//std::thread thread([&]() {
//    while (true)
//    {
//        {
//            // 显示图像
//            cv::imshow("Image", dstImage);
//        }
//
//        // 等待 30 毫秒
//        int key = cv::waitKey(30);
//
//        if (key == 27)   // 如果用户按下 ESC 键，则退出循环
//            break;
//
//        if (clicked)     // 如果已单击鼠标，则清除标志变量并重新加锁互斥量
//        {
//            clicked = 0;
//        }
//    }
//    });