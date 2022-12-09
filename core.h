#ifndef CORE_H
#define CORE_H

#include <opencv2/opencv.hpp>

class Core
{
public:
    static void segmentation(cv::Mat &mat, const int &s, const int &m);

private:
    static cv::Mat *mat;
    static int s;
    static int m;
    static int distance(const std::vector<int> &sxy, const std::vector<int> &pxy);
    static int gradient(const std::vector<int> &pxy);
};
#endif // CORE_H
