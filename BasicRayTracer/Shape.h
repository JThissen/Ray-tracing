#pragma once
#include "Vec3.h"
#include "Matrix44.h"

class Shape
{
public:
	float ior;
	float Kd, Ks;
	Vec3f diffuseColor;
	float specularExponent;
	Matrix44f objectToWorldTransform;

	Shape() : ior{1.3f}, Kd{0.8f}, Ks{0.2f}, specularExponent{ 1250 }{}
	virtual ~Shape() {}
	virtual bool intersect(const Vec3f &rayOrigin, const Vec3f &rayDirection, float &t) const = 0; // const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, float &u, float &v
	virtual void getData(const Vec3f &point, Vec3f &normal, Vec3f &surfaceColor, Vec3f &emissionColor, float &transparency, float &reflection) const = 0;
};