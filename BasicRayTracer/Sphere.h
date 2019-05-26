#pragma once
#include "Shape.h"

class Sphere : public Shape
{
public:
	Vec3f center;                          
	float radius;                
	Vec3f surfaceColor;
	Vec3f emissionColor;      
	float transparency;
	float reflection;

	Sphere(const Vec3f &c, const float &r, const Vec3f &sc = 0, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0) :
		center(c), radius(r), surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl), Shape{}{}

	virtual void getData(const Vec3f &point, Vec3f &normal, Vec3f &surfaceColor, Vec3f &emissionColor, float &transparency, 
		float &reflection) const override
	{
		normal = (point - center).normalize();
		surfaceColor = this->surfaceColor;
		emissionColor = this-> emissionColor;
		transparency = this->transparency;
	}

	bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
	{
		float discr = b * b - 4 * a * c;
		if (discr < 0) return false;
		else if (discr == 0) x0 = x1 = -0.5f * b / a;
		else {
			float q = (b > 0) ?
				-0.5f * (b + sqrt(discr)) :
				-0.5f * (b - sqrt(discr));
			x0 = q / a;
			x1 = c / q;
		}
		if (x0 > x1) std::swap(x0, x1);
		return true;
	}

	virtual bool intersect(const Vec3f &rayOrigin, const Vec3f &rayDirection, float &t) const override //, const Vec3f &v0,const Vec3f &v1, const Vec3f &v2, float &u, float &v
	{
		Vec3f oc = rayOrigin - center;
		float a = rayDirection.dotProduct(rayDirection);
		float b = 2 * rayDirection.dotProduct(oc);
		float c = oc.dotProduct(oc) - radius * radius;
		float t0, t1;
		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0)
			return false;

		else if (discriminant == 0) 
			t0 = t1 = -0.5f * b / a;
		else 
		{
			float q = (b > 0) ?
				-0.5f * (b + sqrt(discriminant)) :
				-0.5f * (b - sqrt(discriminant));
			t0 = q / a;
			t1 = c / q;
		}
		if (t0 > t1) std::swap(t0, t1);
		
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		t = t0;

		return true;
	}
};