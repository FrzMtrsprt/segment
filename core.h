#ifndef CORE_H
#define CORE_H

#include <opencv2/opencv.hpp>

class Core
{
public:
    static void segmentation(cv::Mat &mat);

private:
    static int s;
    static int m;
    static int distance(const cv::Mat &mat, const std::vector<int> &sxy, const std::vector<int> &pxy);
};
#endif // CORE_H
