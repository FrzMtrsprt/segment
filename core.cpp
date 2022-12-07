#include "core.h"

#include <opencv2/opencv.hpp>

// Sampling step
int Core::s = 100;
// Distance constant
int Core::m = 10;

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

    // Each Pixel's seed number and distance
    std::vector<std::vector<std::vector<int>>> cluster;
    // Fill the 3d vector
    for (int x = 0; x <= mat.rows; x++)
    {
        std::vector<std::vector<int>> row;
        for (int y = 0; y <= mat.cols; y++)
        {
            // An invalid seed number and maximum distance
            row.push_back({-1, INT_MAX});
        }
        cluster.push_back(row);
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
                const int d = distance(mat, seed, {x, y});
                if (d < cluster[x][y][1])
                {
                    cluster[x][y] = {i, d};
                }
            }
        }
    }

    // Paint every pixel
    for (int x = 0; x < mat.rows; x++)
    {
        for (int y = 0; y < mat.cols; y++)
        {
            const int i = cluster[x][y][0];
            // If seed number is valid
            if (i >= 0)
            {
                mat.at<cv::Vec3b>(x, y) = mat.at<cv::Vec3b>(seeds[i][0], seeds[i][1]);
            }
        }
    }

    // Paint the seeds
    for (const std::vector<int> seed : seeds)
    {
        mat.at<cv::Vec3b>(seed[0], seed[1]) = {0, 0, 0};
    }
}

int Core::distance(const cv::Mat &mat, const std::vector<int> &sxy, const std::vector<int> &pxy)
{
    // Color of seed and point
    cv::Vec3b sc = mat.at<cv::Vec3b>(sxy[0], sxy[1]);
    cv::Vec3b pc = mat.at<cv::Vec3b>(pxy[0], pxy[1]);
    // Distance of color
    const double dc = sqrt(pow(sc[0] - pc[0], 2) + pow(sc[1] - pc[1], 2) + pow(sc[2] - pc[2], 2));
    // Distance of space
    const double ds = sqrt(pow(sxy[0] - pxy[0], 2) + pow(sxy[1] - pxy[1], 2));
    // Weighed distance
    const int d = (int)(dc * s + ds * m);
    return d;
}
