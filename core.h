#ifndef CORE_H
#define CORE_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Core
{
public:
    static void segmentation(Mat &mat);
};
#endif // CORE_H