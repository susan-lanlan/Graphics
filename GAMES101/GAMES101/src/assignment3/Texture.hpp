//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        //std::cout << u_img << "," << v_img << std::endl;
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }
    Eigen::Vector3f lerp(float x, Eigen::Vector3f v0, Eigen::Vector3f v1) {
        return v0 + x * (v1 - v0);
    }
    Eigen::Vector3f getPixel(int x, int y) {
        auto color = image_data.at<cv::Vec3b>(x, y);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }
    Eigen::Vector3f getColorBilinear(float u, float v){
        u = u > 1 ? 1 : u;
        u = u < 0 ? 0 : u;
        v = v > 1 ? 1 : v;
        v = v < 0 ? 0 : v;
        float x = u * width;
        float y = (1 - v) * height;
        int xmin = u * width;
        int ymin = (1 - v) * height;
        float s = x - xmin;
        float t = y - ymin;
        Eigen::Vector3f u0 = lerp(s, getPixel(xmin, ymin), getPixel(xmin + 1, ymin));
        Eigen::Vector3f u1 = lerp(s, getPixel(xmin, ymin + 1), getPixel(xmin + 1, ymin + 1));
        Eigen::Vector3f color = lerp(t, u0, u1);
        return color;
    }

};
#endif //RASTERIZER_TEXTURE_H
