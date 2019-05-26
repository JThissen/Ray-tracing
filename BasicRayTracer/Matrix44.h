#pragma once
#include "Vec3.h"

template<typename T>
class Matrix44
{
public:

	T x[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };

	Matrix44() {}

	Matrix44(T a, T b, T c, T d, T e, T f, T g, T h,
		T i, T j, T k, T l, T m, T n, T o, T p)
	{
		x[0][0] = a;
		x[0][1] = b;
		x[0][2] = c;
		x[0][3] = d;
		x[1][0] = e;
		x[1][1] = f;
		x[1][2] = g;
		x[1][3] = h;
		x[2][0] = i;
		x[2][1] = j;
		x[2][2] = k;
		x[2][3] = l;
		x[3][0] = m;
		x[3][1] = n;
		x[3][2] = o;
		x[3][3] = p;
	}

	const T* operator [] (uint8_t i) const { return x[i]; }
	T* operator [] (uint8_t i) { return x[i]; }

	Matrix44 operator * (const Matrix44& v) const
	{
		Matrix44 tmp;
		multiply(*this, v, tmp);
		return tmp;
	}

	static void multiply(const Matrix44<T> &a, const Matrix44& b, Matrix44 &c)
	{
		for (uint8_t i = 0; i < 4; ++i) {
			for (uint8_t j = 0; j < 4; ++j) {
				c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] +
					a[i][2] * b[2][j] + a[i][3] * b[3][j];
			}
		}
	}

	Matrix44 transposed() const
	{
		Matrix44 t;
		for (uint8_t i = 0; i < 4; ++i) {
			for (uint8_t j = 0; j < 4; ++j) {
				t[i][j] = x[j][i];
			}
		}
	}

	Matrix44& transpose()
	{
		Matrix44 tmp(x[0][0],
			x[1][0],
			x[2][0],
			x[3][0],
			x[0][1],
			x[1][1],
			x[2][1],
			x[3][1],
			x[0][2],
			x[1][2],
			x[2][2],
			x[3][2],
			x[0][3],
			x[1][3],
			x[2][3],
			x[3][3]);
		*this = tmp;

		return *this;
	}

	template<typename S>
	void multiplyPointMatrix(const Vec3<S> &src, Vec3<S> &dst) const
	{
		S a, b, c, w;

		a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0] + x[3][0];
		b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1] + x[3][1];
		c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2] + x[3][2];
		w = src[0] * x[0][3] + src[1] * x[1][3] + src[2] * x[2][3] + x[3][3];

		dst.x = a / w;
		dst.y = b / w;
		dst.z = c / w;
	}

	template<typename S>
	void multiplyVectorMatrix(const Vec3<S> &src, Vec3<S> &dst) const
	{
		S a, b, c;

		a = src[0] * x[0][0] + src[1] * x[1][0] + src[2] * x[2][0];
		b = src[0] * x[0][1] + src[1] * x[1][1] + src[2] * x[2][1];
		c = src[0] * x[0][2] + src[1] * x[1][2] + src[2] * x[2][2];

		dst.x = a;
		dst.y = b;
		dst.z = c;
	}

	//Matrix44 inverse() const
	//{
	//	int i, j, k;
	//	Matrix44 s;
	//	Matrix44 t(*this);

	//	// Forward elimination
	//	for (i = 0; i < 3; i++) {
	//		int pivot = i;

	//		T pivotsize = t[i][i];

	//		if (pivotsize < 0)
	//			pivotsize = -pivotsize;

	//		for (j = i + 1; j < 4; j++) {
	//			T tmp = t[j][i];

	//			if (tmp < 0)
	//				tmp = -tmp;

	//			if (tmp > pivotsize) {
	//				pivot = j;
	//				pivotsize = tmp;
	//			}
	//		}

	//		if (pivotsize == 0) {
	//			// Cannot invert singular matrix
	//			return Matrix44();
	//		}

	//		if (pivot != i) {
	//			for (j = 0; j < 4; j++) {
	//				T tmp;

	//				tmp = t[i][j];
	//				t[i][j] = t[pivot][j];
	//				t[pivot][j] = tmp;

	//				tmp = s[i][j];
	//				s[i][j] = s[pivot][j];
	//				s[pivot][j] = tmp;
	//			}
	//		}

	//		for (j = i + 1; j < 4; j++) {
	//			T f = t[j][i] / t[i][i];

	//			for (k = 0; k < 4; k++) {
	//				t[j][k] -= f * t[i][k];
	//				s[j][k] -= f * s[i][k];
	//			}
	//		}
	//	}

	//	// Backward substitution
	//	for (i = 3; i >= 0; --i) {
	//		T f;

	//		if ((f = t[i][i]) == 0) {
	//			// Cannot invert singular matrix
	//			return Matrix44();
	//		}

	//		for (j = 0; j < 4; j++) {
	//			t[i][j] /= f;
	//			s[i][j] /= f;
	//		}

	//		for (j = 0; j < i; j++) {
	//			f = t[j][i];

	//			for (k = 0; k < 4; k++) {
	//				t[j][k] -= f * t[i][k];
	//				s[j][k] -= f * s[i][k];
	//			}
	//		}
	//	}

	//	return s;
	//}

	//const Matrix44<T>& invert()
	//{
	//	*this = inverse();
	//	return *this;
	//}

	//friend std::ostream& operator << (std::ostream &s, const Matrix44 &m)
	//{
	//	std::ios_base::fmtflags oldFlags = s.flags();
	//	int width = 12; // total with of the displayed number
	//	s.precision(5); // control the number of displayed decimals
	//	s.setf(std::ios_base::fixed);

	//	s << "[" << std::setw(width) << m[0][0] <<
	//		" " << std::setw(width) << m[0][1] <<
	//		" " << std::setw(width) << m[0][2] <<
	//		" " << std::setw(width) << m[0][3] << "\n" <<

	//		" " << std::setw(width) << m[1][0] <<
	//		" " << std::setw(width) << m[1][1] <<
	//		" " << std::setw(width) << m[1][2] <<
	//		" " << std::setw(width) << m[1][3] << "\n" <<

	//		" " << std::setw(width) << m[2][0] <<
	//		" " << std::setw(width) << m[2][1] <<
	//		" " << std::setw(width) << m[2][2] <<
	//		" " << std::setw(width) << m[2][3] << "\n" <<

	//		" " << std::setw(width) << m[3][0] <<
	//		" " << std::setw(width) << m[3][1] <<
	//		" " << std::setw(width) << m[3][2] <<
	//		" " << std::setw(width) << m[3][3] << "]";

	//	s.flags(oldFlags);
	//	return s;
	//}
};

typedef Matrix44<float> Matrix44f;