#pragma once
#include "Shape.h"

class Triangle : public Shape
{
public:
	float kEpsilon = 1e-8;
	vector<Vec3f> vertsArray;
	Vec3f surfaceColor;
	Vec3f emissionColor;
	float transparency;
	float reflection;

	/*Triangle(vector<Vec3f> verts, const Vec3f &sc = 0, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0) :
		surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl), Shape{}{}

	virtual bool intersect(const Vec3f &rayOrigin, const Vec3f &rayDirection, float &t, const Vec3f &v0,
		const Vec3f &v1, const Vec3f &v2, float &u, float &v) const override
	{
		Vec3f v0v1 = v1 - v0;
		Vec3f v0v2 = v2 - v0;
		Vec3f N = v0v1.crossProduct(v0v2);
		float denom = N.dotProduct(N);
		float NdotRayDirection = N.dotProduct(rayDirection);
		if (fabs(NdotRayDirection) < kEpsilon)
			return false;

		float d = N.dotProduct(v0);

		t = (N.dotProduct(rayOrigin) + d) / NdotRayDirection;
		if (t < 0) return false;
		Vec3f P = rayOrigin + t * rayDirection;

		Vec3f C;
		Vec3f edge0 = v1 - v0;
		Vec3f vp0 = P - v0;
		C = edge0.crossProduct(vp0);
		if (N.dotProduct(C) < 0) 
			return false;

		Vec3f edge1 = v2 - v1;
		Vec3f vp1 = P - v1;
		C = edge1.crossProduct(vp1);
		if ((u = N.dotProduct(C)) < 0)  
			return false;

		Vec3f edge2 = v0 - v2;
		Vec3f vp2 = P - v2;
		C = edge2.crossProduct(vp2);
		if ((v = N.dotProduct(C)) < 0) 
			return false; 

		u /= denom;
		v /= denom;

		return true; 
	}

	virtual void getData(const Vec3f &point, Vec3f &normal, Vec3f &surfaceColor, Vec3f &emissionColor, float &transparency, float &reflection) const override
	{

	}*/
};