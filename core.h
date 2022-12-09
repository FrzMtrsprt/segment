#ifndef CORE_H
#define CORE_H

#include <opencv2/opencv.hpp>

class Core
{
public:
    Core();
    void segmentation();
    cv::Mat mat;
    int s;
    int m;

private:
    int distance(const std::vector<int> &sxy, const std::vector<int> &pxy);
    int gradient(const std::vector<int> &pxy);
};
#endif // CORE_H
