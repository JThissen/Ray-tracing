#pragma once
//#include "geometry.h"
#include "Vec3.h"
#include "Matrix44.h"
#include <limits>
#include <random>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cmath>
using namespace std;

random_device rd2;
mt19937 generator2(rd2());
uniform_real_distribution<float> distribution2(-1, 1);

class Light
{
public:
	float intensity;
	Vec3f color;
	Vec3f position;
	Matrix44f lightToWorldTransform;

	Light(const Matrix44f &l2wt, const Vec3f &c = Vec3f(1), const float &i = 1) : intensity{i}, color{c}
	{
		lightToWorldTransform = l2wt;
		lightToWorldTransform.multiplyPointMatrix(Vec3f(0, 0, 0), position);
	}

	void setLightParameters(const Vec3f &point, Vec3f &lightDirection, float &shadowRayMaxDistance) const
	{
		Vec3f randomInUnitSphere = Vec3f(distribution2(generator2), distribution2(generator2), distribution2(generator2));
		float radius2 = ((position + randomInUnitSphere) - point).norm();
		shadowRayMaxDistance = sqrtf(radius2);
		lightDirection = ((position + randomInUnitSphere) - point).normalize();
	}

	float getIntensity() const { return intensity; }
};