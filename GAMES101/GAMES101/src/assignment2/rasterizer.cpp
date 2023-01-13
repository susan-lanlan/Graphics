// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    ////TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Eigen::Vector3f p{ (float)x ,(float)y ,0};
    Eigen::Vector3f a{ _v[0].x() ,_v[0].y(),0 };
    Eigen::Vector3f b{ _v[1].x() ,_v[1].y(),0 };
    Eigen::Vector3f c{ _v[2].x() ,_v[2].y(),0 };
    Eigen::Vector3f ap = p - a; 
    Eigen::Vector3f bp = p - b; 
    Eigen::Vector3f cp = p - c;
    Eigen::Vector3f ab = b - a;
    Eigen::Vector3f bc = c - b;
    Eigen::Vector3f ca = a - c;
    auto z1 = ap.cross(ab).z();
    auto z2 = bp.cross(bc).z();
    auto z3 = cp.cross(ca).z();
    return z1 * z2 > 0 && z2 * z3 > 0;
    //return true;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { c1,c2,c3 };
}
static float superSampling_3(float x, float y, const Vector3f* v) {
    float sum = 0;
    if (insideTriangle(x , y , v)) {
        sum++;
    }
    if (insideTriangle(x + 0.5, y + 0.5, v)) {
        sum++;
    }
    if (insideTriangle(x + 0.5, y , v)) {
        sum++;
    }
    if (insideTriangle(x , y + 0.5, v)) {
        sum++;
    }
    return sum / 4.0;
}
static float superSampling_2(int n, float x, float y, const Vector3f* v) {
    float step = 1.0 / n / 2.0;
    float sum = 0;
    for (float i = 1; i < n * 2; i += 2) {
        for (float j = 1; j <= n * 2; j += 2) {
            if (insideTriangle(x + i * step, y + j * step, v)) {
                sum++;
            }
        }
    }
    return sum / n / n;
}
static float superSampling_1(int n, float x, float y, const Vector3f* v) {
    float step = 1.0 / (n + 1.0);
    float sum = 0;
    for (float i = 1; i <= n; i++) {
        for (float j = 1; j <= n; j++) {
            if (insideTriangle(x+i*step, y+j*step, v)) {
                sum = sum + 1;
            }
            
        }
    }
    return sum / n /n;
}
void rst::rasterizer::superSampling(int n,int x, int y, const Triangle& t) {
    auto v = t.toVector4();
    int index = 0;
    float step = 1.0 / n / 2.0;
    for (float i = 1; i < n*2; i+=2) {
        for (float j = 1; j <= n*2; j+=2) {
            if (insideTriangle(x + i * step, y + j * step, t.v)) {
                float alpha, beta, gamma;
                std::tie(alpha, beta, gamma) = computeBarycentric2D(i, j, t.v);
                float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;
                if (z_interpolated > depth_buf_sample[get_index(x, y)](index)) {//z越大越近
                    depth_buf_sample[get_index(x, y)](index) = z_interpolated;
                    frame_buf_sample[get_index(x, y)].row(index) = t.getColor();//矩阵行赋值
                }
            }
            index += 1;
        }
    }
    
}



void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v1[] = {
                model* to_vec4(buf[i[0]], 1.0f),
                model* to_vec4(buf[i[1]], 1.0f),
                model* to_vec4(buf[i[2]], 1.0f)
        };
        bool print = false;
        if (print) {
            //Homogeneous division
            std::cout << "After model: " << std::endl;
            for (auto& vec : v1) {
                vec /= vec.w();
                std::cout << "(" << vec(0) << "," << vec(1) << "," << vec(2) << ")" << std::endl;
            }
            Eigen::Vector4f v2[] = {
                    view * model * to_vec4(buf[i[0]], 1.0f),
                    view * model * to_vec4(buf[i[1]], 1.0f),
                    view * model * to_vec4(buf[i[2]], 1.0f)
            };
            std::cout << "After view: " << std::endl;
            for (auto& vec : v2) {
                vec /= vec.w();
                std::cout << "(" << vec(0) << "," << vec(1) << "," << vec(2) << ")" << std::endl;
            }
        }
        Eigen::Vector4f v[] = {
                  mvp * to_vec4(buf[i[0]], 1.0f),
                  mvp * to_vec4(buf[i[1]], 1.0f),
                  mvp * to_vec4(buf[i[2]], 1.0f)
        };
        for (auto& vec : v) {
            vec /= vec.w();
           
        }
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
           
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    int anti = 2;
    int anti_n = 2.0;
    auto v = t.toVector4();
    int xmin = std::min(std::min(v[0].x(), v[1].x()), v[2].x());
    int xmax = std::max(std::max(v[0].x(), v[1].x()), v[2].x());
    int ymin = std::min(std::min(v[0].y(), v[1].y()), v[2].y());
    int ymax = std::max(std::max(v[0].y(), v[1].y()), v[2].y());
    for (int x = xmin; x <= xmax; x++)
        for (int y = ymin; y <= ymax; y++) {
            if (anti== 0) {
                x += 0.5; y += 0.5;
                if (insideTriangle(x, y, t.v)) {
                    float alpha, beta, gamma;
                    std::tie(alpha, beta, gamma) = computeBarycentric2D(x, y, t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    Eigen::Vector3f vt{ (float)x,(float)y,z_interpolated };
                    if (z_interpolated > depth_buf[get_index(x, y)]) {
                        set_pixel(vt, t.getColor());
                        depth_buf[get_index(x, y)] = z_interpolated;
                    }
                }
            }
            // Anti-aliasing-1（有不正常的黑边）
            if (anti == 1) {
                //float rate = superSampling_1(anti_n,x, y, t.v);
                float rate = superSampling_2(anti_n, x, y, t.v);
                //float rate = superSampling_3( x, y, t.v);
                if (rate > 0) {
                    float alpha, beta, gamma;
                    std::tie(alpha, beta, gamma) = computeBarycentric2D(x + 0.5, y + 0.5, t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    Eigen::Vector3f vt{ (float)x,(float)y,z_interpolated };
                    if (z_interpolated > depth_buf[get_index(x, y)]) {
                        set_pixel(vt, t.getColor() * rate);
                        depth_buf[get_index(x, y)] = z_interpolated;
                    }
                }
            }
            if (anti == 2) {
                superSampling(anti_n , x, y, t);
            }
    }
    if (anti == 2) {
        for (int x = xmin; x <= xmax; x++)
            for (int y = ymin; y <= ymax; y++) {
                Eigen::Matrix<float, 4, 3> pixel_colors = frame_buf_sample[get_index(x, y)];
                float r = pixel_colors.col(0).sum() / anti_n / anti_n;
                float g = pixel_colors.col(1).sum() / anti_n / anti_n;
                float b = pixel_colors.col(2).sum() / anti_n / anti_n;
                Eigen::Vector3f color = {r,g,b};
                set_pixel({ (float)x,(float)y, 0 }, color);
            }
    }
    
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(frame_buf_sample.begin(), frame_buf_sample.end(), Eigen::Matrix<float, 4, 3>{ {0,0,0}, { 0,0,0 }, { 0,0,0 }, { 0,0,0 } });
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::min());
        std::fill(depth_buf_sample.begin(), depth_buf_sample.end(), 
                    Eigen::Vector4f{ std::numeric_limits<float>::min(),
                                     std::numeric_limits<float>::min(),
                                     std::numeric_limits<float>::min(),
                                     std::numeric_limits<float>::min() });
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    frame_buf_sample.resize(w * h);
    depth_buf_sample.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on