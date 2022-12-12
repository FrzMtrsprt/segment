#include "core.h"

#include <opencv2/opencv.hpp>

Core::Core() {}

void Core::segmentation()
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

    // Each Pixel's seed number and distance
    std::vector<std::vector<std::vector<int>>> pixelInfo;
    // Fill the 3d vector
    for (int x = 0; x <= mat.rows; x++)
    {
        std::vector<std::vector<int>> row;
        for (int y = 0; y <= mat.cols; y++)
        {
            // An invalid seed number and maximum distance
            row.push_back({-1, INT_MAX});
        }
        pixelInfo.push_back(row);
    }

    // Iterate but keep seeds from getting too close
    bool changed = true;
    for (int iter = 0; iter < s / 2 && changed == true; iter++)
    {
        changed = false;
        // For each seed, from 3x3 pixels around it,
        // choose the pixel with the least grad as new seed
        for (int i = 0; i < seeds.size(); i++)
        {
            const std::vector<int> seed = seeds[i];
            std::vector<int> seedMin = {-1, -1};
            int gradMin = INT_MAX;

            const int xstart = std::max(seed[0] - 1, 1);
            const int xend = std::min(seed[0] + 1, mat.rows - 1);
            for (int x = xstart; x < xend; x++)
            {
                const int ystart = std::max(seed[1] - 1, 1);
                const int yend = std::min(seed[1] + 1, mat.cols - 1);
                for (int y = ystart; y < yend; y++)
                {
                    const int grad = gradient({x, y});
                    if (grad < gradMin)
                    {
                        seedMin = {x, y};
                        gradMin = grad;
                    }
                }
            }
            // If seed moved
            if (seedMin[0] != seeds[i][0] || seedMin[1] != seeds[i][1])
            {
                seeds[i] = seedMin;
                changed = true;
            }
        }
    }

    // For each seed, cluster the pixels around it
    for (int i = 0; i < seeds.size(); i++)
    {
        const std::vector<int> seed = seeds[i];
        const int xstart = std::max(seed[0] - s, 0);
        const int xend = std::min(seed[0] + s, mat.rows);
        for (int x = xstart; x < xend; x++)
        {
            const int ystart = std::max(seed[1] - s, 0);
            const int yend = std::min(seed[1] + s, mat.cols);
            for (int y = ystart; y < yend; y++)
            {
                const int d = distance(seed, {x, y});
                if (d < pixelInfo[x][y][1])
                {
                    // Assign the pixel to new seed
                    pixelInfo[x][y] = {i, d};
                }
            }
        }
    }

    // Paint every pixel
    for (int x = 0; x < mat.rows; x++)
    {
        for (int y = 0; y < mat.cols; y++)
        {
            const int i = pixelInfo[x][y][0];
            // If seed number is valid
            if (i >= 0)
            {
                mat.at<cv::Vec3b>(x, y) = mat.at<cv::Vec3b>(seeds[i][0], seeds[i][1]);
            }
            else
            {
                mat.at<cv::Vec3b>(x, y) = {0, 255, 0}; // Green
            }
        }
    }

    // Paint the seeds
    for (const std::vector<int> &seed : seeds)
    {
        mat.at<cv::Vec3b>(seed[0], seed[1]) = {0, 0, 0};
    }
}

int Core::distance(const std::vector<int> &sxy, const std::vector<int> &pxy)
{
    // Color of seed and point
    const cv::Vec3b sc = mat.at<cv::Vec3b>(sxy[0], sxy[1]);
    const cv::Vec3b pc = mat.at<cv::Vec3b>(pxy[0], pxy[1]);
    // Distance of color
    const double dc = (abs(sc[0] - pc[0]) + abs(sc[1] - pc[1]) + abs(sc[2] - pc[2])) / 3;
    // Distance of space
    const double ds = (abs(sxy[0] - pxy[0]) + abs(sxy[1] - pxy[1])) / 2;
    // Weighed distance
    const int d = (int)(dc * s + ds * m);
    return d;
}

int Core::gradient(const std::vector<int> &pxy)
{
    // P and the four pixels around it
    const cv::Vec3b p = mat.at<cv::Vec3b>(pxy[0], pxy[1]);
    const cv::Vec3b xlo = mat.at<cv::Vec3b>(pxy[0] - 1, pxy[1]);
    const cv::Vec3b xhi = mat.at<cv::Vec3b>(pxy[0] + 1, pxy[1]);
    const cv::Vec3b ylo = mat.at<cv::Vec3b>(pxy[0], pxy[1] - 1);
    const cv::Vec3b yhi = mat.at<cv::Vec3b>(pxy[0], pxy[1] + 1);
    const int dxhi = abs(xhi[0] - p[0]) + abs(xhi[1] - p[1]) + abs(xhi[2] - p[2]);
    const int dxlo = abs(xlo[0] - p[0]) + abs(xlo[1] - p[1]) + abs(xlo[2] - p[2]);
    const int dyhi = abs(yhi[0] - p[0]) + abs(yhi[1] - p[1]) + abs(yhi[2] - p[2]);
    const int dylo = abs(ylo[0] - p[0]) + abs(ylo[1] - p[1]) + abs(ylo[2] - p[2]);
    return dxhi + dxlo + dyhi + dylo;
}
