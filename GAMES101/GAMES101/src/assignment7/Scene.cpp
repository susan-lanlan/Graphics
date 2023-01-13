//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include "Material.hpp"

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

Vector3f Scene::shade(const Intersection &p, Vector3f wo) const
{
    if (p.m->hasEmission())
    { 
        //std::cout << p.m->getEmission() << std::endl;
        return p.m->getEmission();
    }
        

    Vector3f Ldir, LIndir;
    wo=-wo;
    Intersection interLight;float pdfLight;
    sampleLight(interLight, pdfLight);//在函数里计算这两个变量

    Vector3f x = interLight.coords;
    Vector3f ws = x - p.coords;//着色点到光源的方向

    float distance2 = dotProduct(ws, ws); //光源点到着色点的距离平方
    
    ws = normalize(ws);
    Vector3f emit = interLight.emit;
    Vector3f NN = interLight.normal;
    Vector3f N = p.normal;
    Intersection interP = intersect(Ray(p.coords, ws));
    //着色点向光源发出射线,中间没有被挡住,而不是没有相交 
    if ((interP.coords - x).norm() < EPSILON)
    {
        Ldir = emit * p.m->eval(ws, wo, N) * dotProduct(ws, N) * dotProduct(-ws, NN) / (distance2 * pdfLight);
    }
    if(get_random_float() < RussianRoulette)
    {
        Vector3f wi = normalize(p.m->sample(wo, N));
        Ray ray(p.coords, wi);
        Intersection interNoEmit = Scene::intersect(ray);
        if (interNoEmit.happened && !interNoEmit.m->hasEmission())
        {
            LIndir = shade(interNoEmit,wi) * p.m->eval(ws, wo, N) * dotProduct(wi, N) / p.m->pdf(wi, wo, N) / RussianRoulette;
        }
        
    }
    return Ldir + LIndir;

}
 //Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    if (depth > this->maxDepth) {
        return Vector3f(0.0, 0.0, 0.0);
    }
    Intersection inter = Scene::intersect(ray);
    Vector3f hitColor = Vector3f(0.0, 0.0, 0.0);
    if (inter.happened) {
        hitColor = shade(inter, ray.direction);
        //hitColor = { 1,1,1 };
    }

    return hitColor;

}
// yy
//Vector3f Scene::castRay(const Ray& ray, int depth) const
//{
//    if (depth > this->maxDepth) {
//        return Vector3f(0.0, 0.0, 0.0);
//    }
//    Intersection p = Scene::intersect(ray);
//    Vector3f hitColor = Vector3f(0.0, 0.0, 0.0);
//    if (p.happened) {
//        if (p.m->hasEmission())
//            return p.m->getEmission();
//
//        Vector3f Ldir, LIndir;
//        Vector3f wo = -ray.direction;
//        Intersection interLight; float pdfLight;
//        sampleLight(interLight, pdfLight);//在函数里计算这两个变量
//
//        Vector3f x = interLight.coords;
//        Vector3f ws = x - p.coords;//光源到着色点的方向
//
//        float distance2 = dotProduct(ws, ws); //光源点到着色点的距离平方
//
//        ws = normalize(ws);
//        Vector3f emit = interLight.emit;
//        Vector3f NN = interLight.normal;
//        Vector3f N = p.normal;
//        Intersection interObject = intersect(Ray(p.coords, ws));
//        if ((interObject.coords-x).norm() < EPSILON)//作色点向光源发出射线 
//        {
//            Ldir = emit * p.m->eval(ws, wo, N) * dotProduct(ws, N) * dotProduct(ws, NN) / distance2 / pdfLight;
//            //-ws,不加负号点会非常稀疏
//        }
//        if (get_random_float() < RussianRoulette)
//        {
//            Vector3f wi = normalize(p.m->sample(wo, N));
//            Ray ray(p.coords + N * EPSILON, wi);
//            LIndir = castRay(ray, depth + 1) * p.m->eval(wi, wo, N) * dotProduct(wi, N) / p.m->pdf(wi, wo, N) / RussianRoulette;
//            
//        }
//        hitColor = Ldir + LIndir;
//    }
//
//    return hitColor;
//}