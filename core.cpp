#include "core.h"

#include <opencv2/opencv.hpp>

cv::Mat *Core::mat = nullptr;

// Sampling step
int Core::s = 50;
// Distance constant
int Core::m = 10;

void Core::segmentation(cv::Mat &mat, const int &s, const int &m)
{
    Core::mat = &mat;
    Core::s = s;
    Core::m = m;

    // Initialize seed points
    std::vector<std::vector<int>> seeds;
    for (int i = s / 2; i < mat.rows; i += s)
    {
        for (int j = s / 2; j < mat.cols; j += s)
        {
            seeds.push_back({i, j});
        }
    }

    // For each seed, from 3x3 pixels around it,
    // choose the pixel with the least grad as new seed
    for (int i = 0; i < seeds.size(); i++)
    {
        const std::vector<int> seed = seeds[i];
        std::vector<int> seedMin = {-1, -1};
        int gradMin = INT_MAX;

        const int xstart = std::max(seed[0] - 1, 1);
        const int xend = std::min(seed[0] + 1, mat.rows - 1);
        for (int x = xstart; x <= xend; x++)
        {
            const int ystart = std::max(seed[1] - 1, 1);
            const int yend = std::min(seed[1] + 1, mat.cols - 1);
            for (int y = ystart; y <= yend; y++)
            {
                const int grad = gradient({x, y});
                if (grad < gradMin)
                {
                    seedMin = {x, y};
                    gradMin = grad;
                }
            }
        }
        seeds[i] = seedMin;
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
                const int d = distance(seed, {x, y});
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

int Core::distance(const std::vector<int> &sxy, const std::vector<int> &pxy)
{
    // Color of seed and point
    const cv::Vec3b sc = mat->at<cv::Vec3b>(sxy[0], sxy[1]);
    const cv::Vec3b pc = mat->at<cv::Vec3b>(pxy[0], pxy[1]);
    // Distance of color
    const double dc = (pow(sc[0] - pc[0], 2) + pow(sc[1] - pc[1], 2) + pow(sc[2] - pc[2], 2)) / 3;
    // Distance of space
    const double ds = (pow(sxy[0] - pxy[0], 2) + pow(sxy[1] - pxy[1], 2)) / 2;
    // Weighed distance
    const int d = (int)(dc * s + ds * m);
    return d;
}

int Core::gradient(const std::vector<int> &pxy)
{
    // The four pixels around p
    const cv::Vec3b xlo = mat->at<cv::Vec3b>(pxy[0] - 1, pxy[1]);
    const cv::Vec3b xhi = mat->at<cv::Vec3b>(pxy[0] + 1, pxy[1]);
    const cv::Vec3b ylo = mat->at<cv::Vec3b>(pxy[0], pxy[1] - 1);
    const cv::Vec3b yhi = mat->at<cv::Vec3b>(pxy[0], pxy[1] + 1);
    const int dx = (xhi[0] - xlo[0] + xhi[1] - xlo[1] + xhi[2] - xlo[2]) / 3;
    const int dy = (yhi[0] - ylo[0] + yhi[1] - ylo[1] + yhi[2] - ylo[2]) / 3;
    return dx + dy;
}
