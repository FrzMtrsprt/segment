#include "core.h"

#include <opencv2/opencv.hpp>

int s = 100;

void Core::segmentation(cv::Mat &mat)
{
    // Initialize seed points
    std::vector<std::vector<int>> seeds;
    for (int i = s / 2; i < mat.rows; i += s)
    {
        for (int j = s / 2; j < mat.cols; j += s)
        {
            seeds.push_back({i, j});
        }
    }
    for (std::vector<int> seed : seeds)
    {

        mat.at<int>(seed[0], seed[1]) = 0;
    }
}