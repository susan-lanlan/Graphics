//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H
#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>

class Bounds3
{
  public:
    Vector3f pMin, pMax; // two points to specify the bounding box
    Bounds3()
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = Vector3f(minNum, minNum, minNum);
        pMin = Vector3f(maxNum, maxNum, maxNum);
    }
    Bounds3(const Vector3f p) : pMin(p), pMax(p) {}
    Bounds3(const Vector3f p1, const Vector3f p2)
    {
        pMin = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    Vector3f Diagonal() const { return pMax - pMin; }
    int maxExtent() const
    {
        Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    double SurfaceArea() const
    {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    Vector3f Centroid() { return 0.5 * pMin + 0.5 * pMax; }
    Bounds3 Intersect(const Bounds3& b)
    {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),
                                fmax(pMin.z, b.pMin.z)),
                       Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),
                                fmin(pMax.z, b.pMax.z)));
    }

    Vector3f Offset(const Vector3f& p) const
    {
        Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    bool Overlaps(const Bounds3& b1, const Bounds3& b2)
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    bool Inside(const Vector3f& p, const Bounds3& b)
    {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    inline const Vector3f& operator[](int i) const
    {
        return (i == 0) ? pMin : pMax;
    }

    inline bool IntersectP(const Ray& ray, const Vector3f& invDir,
                           const std::array<int, 3>& dirisNeg) const;
    inline bool IntersectRay(const Ray& ray) const;
};

inline bool Bounds3::IntersectRay(const Ray& ray) const
{
    // 判断光线是否与 AABB 相交
    // 求解光线与三对平面的相交时间点
    // 逻辑判断是否相交

    // x-axis
    float tx1, tx2;
    tx1 = (pMin.x - ray.origin.x) / ray.direction.x;
    tx2 = (pMax.x - ray.origin.x) / ray.direction.x;
    if (tx1 > tx2) std::swap(tx1, tx2);

    // y-axis
    float ty1, ty2;
    ty1 = (pMin.y - ray.origin.y) / ray.direction.y;
    ty2 = (pMax.y - ray.origin.y) / ray.direction.y;
    if (ty1 > ty2) std::swap(ty1, ty2);

    // z-axis
    float tz1, tz2;
    tz1 = (pMin.z - ray.origin.z) / ray.direction.z;
    tz2 = (pMax.z - ray.origin.z) / ray.direction.z;
    if (tz1 > tz2) std::swap(tz1, tz2);
    float t_exit = std::min(std::min(tx2, ty2), tz2);
    float t_enter = std::max(std::max(tx1, ty1), tz1);
    std::cout << "yy" << std::endl;
    std::cout << Vector3f(tx1, ty1, tz1) << std::endl;
    std::cout << t_enter << std::endl;
    std::cout << Vector3f(tx2, ty2, tz2) << std::endl;
    std::cout << t_exit << std::endl;
    if (std::min(std::min(tx2, ty2), tz2) < 0) return false;

    if (std::max(std::max(tx1, ty1), tz1) > std::min(std::min(tx2, ty2), tz2)) return false;

    return true;
}


inline bool Bounds3::IntersectP(const Ray& ray, const Vector3f& invDir,
                                const std::array<int, 3>& dirIsNeg) const
{
    Vector3f res1 = (pMin - ray.origin) * invDir;
    Vector3f res2 = (pMax - ray.origin) * invDir;
    std::vector<Vector3f> t{ res1,res2 };
    //方向x>0，dirIsNeg[0]=1,则朝着x增大的方向，t_exit则在pMax上，即t[1]=res2
    float t_exit_szl = std::min(std::min(t[dirIsNeg[0]].x, t[dirIsNeg[1]].y), t[dirIsNeg[2]].z);
    if (t_exit_szl < 0)
        return false;
    float t_enter_szl = std::max(std::max(t[1 - dirIsNeg[0]].x, t[1 - dirIsNeg[1]].y), t[1 - dirIsNeg[2]].z);
    return t_exit_szl >= t_enter_szl;
}

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

inline Bounds3 Union(const Bounds3& b, const Vector3f& p)
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
