#pragma once

#include <iostream>
#include <cmath>
using namespace std;

template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3 operator + (const Vec3 &v) const{return Vec3(x + v.x, y + v.y, z + v.z);}
	Vec3 operator - (const Vec3 &v) const{return Vec3(x - v.x, y - v.y, z - v.z);}
	Vec3 operator - () const{return Vec3(-x, -y, -z);}
	Vec3 operator * (const T &r) const { return Vec3(x * r, y * r, z * r); }

	Vec3 operator/ (const T &r) const{	return Vec3(x / r, y / r, z / r);} 
	Vec3 operator * (const Vec3 &v) const{	return Vec3(x * v.x, y * v.y, z * v.z);}
	T dotProduct(const Vec3<T> &v) const{	return x * v.x + y * v.y + z * v.z;}
	Vec3& operator /= (const T &r){	x /= r, y /= r, z /= r; return *this;}
	Vec3& operator *= (const T &r){	x *= r, y *= r, z *= r; return *this;}
	Vec3 crossProduct(const Vec3<T> &v) const{	return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);}
	T norm() const{	return x * x + y * y + z * z;}
	T length() const{	return sqrt(norm());}

	const T& operator [] (uint8_t i) const { return (&x)[i]; }
	T& operator [] (uint8_t i) { return (&x)[i]; }
	Vec3& normalize()
	{
		T n = norm();
		if (n > 0) 
		{
			T factor = 1 / sqrt(n);
			x *= factor, y *= factor, z *= factor;
		}
		return *this;
	}

	friend Vec3 operator * (const T &r, const Vec3 &v){return Vec3<T>(v.x * r, v.y * r, v.z * r);}
	friend Vec3 operator / (const T &r, const Vec3 &v){return Vec3<T>(r / v.x, r / v.y, r / v.z);}
	friend ostream& operator << (ostream &s, const Vec3<T> &v){return s << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';}
};

typedef Vec3<float> Vec3f;