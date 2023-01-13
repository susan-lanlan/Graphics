//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

 /*The main render function. This where we iterate over all pixels in the image,
 generate primary rays and cast these rays into the scene. The content of the
 framebuffer is saved to a file.*/
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(long long(scene.width) * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(-1, 5, 10);
    int m = 0;
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            // TODO: Find the x and y positions of the current pixel to get the
            // direction
            //  vector that passes through it.
            // Also, don't forget to multiply both of them with the variable
            // *scale*, and x (horizontal) variable with the *imageAspectRatio*
            // Don't forget to normalize this direction!
            
            float xn = i + 0.5;
            float yn = j + 0.5;
            float x = (2.0 * xn / scene.width - 1.0) * imageAspectRatio * scale;
            float y = (1.0 - 2.0 * yn / scene.height) * scale;
            Vector3f dir = normalize(Vector3f(x, y, -1.0));// Don't forget to normalize this direction!
            //相机坐标系下，相机在原点
            Ray ray(eye_pos, dir);
            framebuffer[m++] = scene.castRay(ray, 0);

            //MSAA n*n个采样点
            //Vector3f color = { 0,0,0 };
            //int n = 2;
            //float step = 1.0 / n / 2.0;
            //for (float m = 1; m < n * 2; m += 2) {
            //    for (float q = 1; q <= n * 2; q += 2) {
            //        float xn = i + m * step;
            //        float yn = j + q * step;
            //        float x = (2.0 * xn / scene.width - 1) * imageAspectRatio * scale;
            //        float y = (1 - 2.0 * yn / scene.height) * scale;
            //        Vector3f dir = normalize(Vector3f(x, y, -1));// Don't forget to normalize this direction!
            //        Ray ray(eye_pos, dir);
            //        color += scene.castRay(ray,0);
            //    }
            //}
            //framebuffer[m++] = color / n / n / 1.0;

        }
        UpdateProgress(j / (float)scene.height);

    }
    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
