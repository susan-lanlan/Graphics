#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    //假设相机的gaze和up方向已经符合标准,可以反推相机的3个参数

    Eigen::Matrix4f translate;//将相机位置平移到原点
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    //填写一个绕z轴旋转的矩阵，只需要修改矩阵的某些元素即可
    //将旋转（平移缩放等等...）矩阵用于初始模型，即可视为对不同的模型在渲染（拍照中的摆pose）
    model(0, 0) = cos(rotation_angle / 180* MY_PI);
    model(0, 1) = -sin(rotation_angle / 180 * MY_PI);
    model(1, 0) = sin(rotation_angle / 180 * MY_PI);
    model(1, 1) = cos(rotation_angle / 180 * MY_PI);

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    float n = -zNear;
    float f = -zFar;
    float t = zNear * tan(eye_fov / 2 / 180 * MY_PI);
    float r = t * aspect_ratio;
    float b = -t;
    float l = -r;
    Eigen::Matrix4f Mo = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f Mo_s = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f Mo_t = Eigen::Matrix4f::Identity();
    Mo_s(0, 0) = 2 / (r - l);
    Mo_s(1, 1) = 2 / (t - b);
    Mo_s(2, 2) = 2 / (n - f);
    Mo_t(0, 3) = -(r + l) / 2;
    Mo_t(1, 3) = -(t + b) / 2;
    Mo_t(2, 3) = -(n + f) / 2;
    Mo = Mo_s * Mo_t;
    Eigen::Matrix4f Mp2o = Eigen::Matrix4f::Identity();
    Mp2o(0, 0) = n;
    Mp2o(1, 1) = n;
    Mp2o(2, 2) = n+f; Mp2o(2, 3) = -n*f;
    Mp2o(3, 2) = 1; Mp2o(3, 3) = 0;
    projection = Mo * Mp2o;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};//相机的位置

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        //r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);
        //std::cout << "frame count: " << frame_count++ << '\n';
        return 0;
    }

    while (key != 27) {
        //r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(30);
        if (key != -1) {
            std::cout << "key 1: " << key << '\n';
        }
        //std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
            std::cout << "key: " << key << '\n';
        }
        else if (key == 'd') {
            angle -= 10;
            std::cout << "key: " << key << '\n';
        }
    }

    return 0;
}
