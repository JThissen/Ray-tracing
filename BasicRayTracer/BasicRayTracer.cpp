#include "pch.h"
#include "Shape.h"
#include "Sphere.h"
#include "Light.h"
#include "Triangle.h"
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <limits>

#define M_PI 3.14159265358979323846

using namespace std;

random_device rd;
mt19937 generator(rd());
uniform_real_distribution<float> distribution(-0.25, 0.25);

struct Settings
{
	int width;
	int height;
	int antiAlias;
	int shadowRays;
	int maxRayDepth;
	float fov;
	float bias;
	Vec3f bgColorA;
	Vec3f bgColorB;
	Matrix44f cameraToWorldTransform;
};

struct IntersectionInfo
{
	float t;
	const Shape *shape;
};

float mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
}

void fresnel(const Vec3f &incidence, const Vec3f &normal, const float &ior, float &fresnel)
{
	float cosi = incidence.dotProduct(normal);
	float etai = 1, etat = ior;

	if (cosi > 0)
		swap(etai, etat);

	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));

	if (sint >= 1) 
		fresnel = 1;
	else 
	{
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		fresnel = (Rs * Rs + Rp * Rp) / 2;
	}
}

bool findNearestIntersection(const Vec3f &rayorig, const Vec3f &raydir, const vector<unique_ptr<Shape>> &spheres, IntersectionInfo &intersectionInfo)
{
	bool intersect = false;

	for (int i = 0; i < spheres.size(); ++i)
	{
		float t = numeric_limits<float>::max();

		if (spheres[i]->intersect(rayorig, raydir, t) && t < intersectionInfo.t)
		{
			intersectionInfo.t = t;
			intersectionInfo.shape = spheres[i].get();
			intersect = true;
		}
	}
	return intersect;
}

Vec3f computeColor(const Vec3f &rayorig, const Vec3f &raydir, const vector<unique_ptr<Shape>> &spheres, const vector<Light> &lights, const Settings &settings, const int &depth)
{
	IntersectionInfo intersectionInfo;
	intersectionInfo.t = numeric_limits<float>::max();
	intersectionInfo.shape = nullptr;

	if (!findNearestIntersection(rayorig, raydir, spheres, intersectionInfo))
	{
		Vec3f dir = raydir;
		dir.normalize();
		float v = (dir.y + 1) * 0.5;
		Vec3f bgColor = (1 - v)  * Vec3f(1, 1, 1) + v * Vec3f(0, 0, 1);
		return bgColor;
	}

	Vec3f resultingColor = 0;
	Vec3f surfaceColor;
	Vec3f point = rayorig + raydir * intersectionInfo.t;
	Vec3f normal;
	Vec3f emissionColor;
	float transparency;
	float reflection;

	intersectionInfo.shape->getData(point, normal, surfaceColor, emissionColor, transparency, reflection);

	bool insideObject = false;
	if (raydir.dotProduct(normal) > 0)
	{
		normal = -1.0f * normal;
		insideObject = true;
	}
		
	if ((transparency > 0 || reflection > 0) && depth < settings.maxRayDepth)
	{
		float facingratio = -raydir.dotProduct(normal);
		float fresnelEffect = mix(pow(1 - facingratio, 3), 1, 0.3f);
		//fresnel(raydir, normal, intersectionInfo.shape->ior, fresnelEffect);
		Vec3f refldir = (raydir - normal * 2.0f * raydir.dotProduct(normal)).normalize();
		Vec3f reflection = computeColor(point + normal * settings.bias, refldir, spheres, lights, settings, depth + 1);
		Vec3f refraction = 0;

		if (transparency)
		{
			float ior = intersectionInfo.shape->ior, eta = (insideObject) ? ior : 1 / ior;
			float cosi = -normal.dotProduct(raydir);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3f refrdir = (raydir * eta + normal * (eta *  cosi - sqrt(k))).normalize();
			refraction = computeColor(point - normal * settings.bias, refrdir, spheres, lights, settings, depth + 1);
		}
		resultingColor = (reflection * fresnelEffect + refraction * (1 - fresnelEffect) * transparency) * surfaceColor;
	}
	else 
	{
		Vec3f diffuse = 0;
		Vec3f specular = 0;

		for (int i = 0; i < lights.size(); i++)
		{
			IntersectionInfo lightRayInfo;
			Vec3f lightDirection;
			Vec3f lightIntensity;

			float shadowAmount = 0;
			for (int j = 0; j < settings.shadowRays; j++)
			{
				lights[i].setLightParameters(point, lightDirection, lightRayInfo.t);
				int visible = findNearestIntersection(point + normal * settings.bias, lightDirection, spheres, lightRayInfo) ? 0 : 1;
				shadowAmount += visible;
			}

			shadowAmount /= settings.shadowRays;
			diffuse = diffuse + shadowAmount * surfaceColor * lights[i].getIntensity() * max(0.0f, normal.dotProduct(lightDirection));
			Vec3f refldir = (raydir - normal * 2.0f * raydir.dotProduct(normal)).normalize();
			specular = lightIntensity * pow(max(0.0f, refldir.dotProduct(-1.0f * raydir)), intersectionInfo.shape->specularExponent);
		}
		resultingColor = diffuse * intersectionInfo.shape->Kd + specular * intersectionInfo.shape->Ks;
	}

	return resultingColor;
}

void saveToFile(const unique_ptr<Vec3f[]> &image, int size, const Settings &settings)
{
	std::ofstream ofs("./tracerResult.ppm", ios::out | ios::binary);
	ofs << "P6\n" << settings.width << " " << settings.height << "\n255\n";
	for (unsigned i = 0; i < size; ++i) {
		ofs << (unsigned char)(min(float(1), image[i].x) * 255) <<
			(unsigned char)(min(float(1), image[i].y) * 255) <<
			(unsigned char)(min(float(1), image[i].z) * 255);
	}

	ofs.close();
}

void renderScene(const vector<unique_ptr<Shape>> &spheres, const vector<Light> &lights, const Settings &settings)
{
	int size = settings.width * settings.height;
	unique_ptr<Vec3f[]> image = unique_ptr<Vec3f[]>(new Vec3f[size]);
	Vec3f *pixel = image.get();
	float invWidth = 1 / float(settings.width), invHeight = 1 / float(settings.height);
	float angle = tan(M_PI * 0.5f * settings.fov / 180.0f);
	float aspectratio = settings.width / float(settings.height);
	Vec3f rayOrigin;
	settings.cameraToWorldTransform.multiplyPointMatrix(Vec3f(0), rayOrigin);

	for (int i = 0; i < settings.height; i++) {
		for (int j = 0; j < settings.width; j++) 
		{
			Vec3f color = Vec3f(0);

			for (int k = 0; k < settings.antiAlias; k++)
			{
				float x = (2.0f * ((j + 0.5f + distribution(generator)) * invWidth) - 1.0f) * angle * aspectratio;
				float y = (1.0f - 2.0f * ((i + 0.5f + distribution(generator)) * invHeight)) * angle;
				Vec3f rayDirection;
				settings.cameraToWorldTransform.multiplyVectorMatrix(Vec3f(x, y, -1), rayDirection);
				rayDirection.normalize();
				color = color + computeColor(rayOrigin, rayDirection, spheres, lights, settings, 0);
			}
			*pixel = (color /(float)settings.antiAlias);
			pixel++;
		}
		cout << "\r" << setprecision(3) << (i / (float)settings.height) * 100.0f << "%";
	}

	saveToFile(image, size, settings);
}

Matrix44f cameraLookAt(const Vec3f &from, const Vec3f &to)
{
	Vec3f yytemp = Vec3f(0, 1, 0);
	Vec3f zz = (from - to).normalize();
	Vec3f xx = yytemp.crossProduct(zz);
	Vec3f yy = zz.crossProduct(xx);

	Matrix44f cameraToWorldTransform;
	cameraToWorldTransform[0][0] = xx.x;
	cameraToWorldTransform[0][1] = xx.y;
	cameraToWorldTransform[0][2] = xx.z;
	cameraToWorldTransform[1][0] = yy.x;
	cameraToWorldTransform[1][1] = yy.y;
	cameraToWorldTransform[1][2] = yy.z;
	cameraToWorldTransform[2][0] = zz.x;
	cameraToWorldTransform[2][1] = zz.y;
	cameraToWorldTransform[2][2] = zz.z;
	cameraToWorldTransform[3][0] = from.x;
	cameraToWorldTransform[3][1] = from.y;
	cameraToWorldTransform[3][2] = from.z;
	return cameraToWorldTransform;
}

int main(int argc, char **argv)
{
	Settings settings;
	settings.width = 640;
	settings.height = 360;
	settings.fov = 90.0f;
	settings.bias = 0.0001f;
	settings.maxRayDepth = 10;
	settings.antiAlias = 5;
	settings.shadowRays = 40;

	Vec3f from = Vec3f(0.0f, 20.0f, 0.0f);
	Vec3f to = Vec3f(0.0f, 10.0f, -20.0f);

	settings.cameraToWorldTransform = cameraLookAt(from, to);

	vector<unique_ptr<Shape>> spheres;
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, -102.0f, -40.0f), 100.0f, Vec3f(1.00f, 1.00f, 1.00f), 0.0f, 0.0f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(-135.0f, 0.0f, -40.0f), 100.0f, Vec3f(0.2, 0.2, 0.8), 0.0f, 0.0f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(135.0f, 0.0f, -40.0f), 100.0f, Vec3f(0.2, 0.8, 0.2), 0.0f, 0.0f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, 0.0f, -360.0f), 300.0f, Vec3f(0.1, 0.1, 0.1), 0.0f, 0.0f)));

	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(15.0f, 0.0f, -20.0f), 5, Vec3f(0.32f, 1.00f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(15.0f, 0.0f, -30.0f), 5, Vec3f(0.32f, 1.00f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(15.0f, 0.0f, -40.0f), 5, Vec3f(0.32f, 1.00f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(15.0f, 0.0f, -50.0f), 5, Vec3f(0.32f, 1.00f, 0.36f), 1.0f, 0.8f)));

	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, 0.0f, -20.0f), 5, Vec3f(1.00f, 0.32f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, 0.0f, -30.0f), 5, Vec3f(1.00f, 0.32f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, 0.0f, -40.0f), 5, Vec3f(1.00f, 0.32f, 0.36f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(0.0f, 0.0f, -50.0f), 5, Vec3f(1.00f, 0.32f, 0.36f), 1.0f, 0.8f)));

	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(-15.0f, 0.0f, -20.0f), 5, Vec3f(0.32f, 0.32f, 1.00f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(-15.0f, 0.0f, -30.0f), 5, Vec3f(0.32f, 0.32f, 1.00f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(-15.0f, 0.0f, -40.0f), 5, Vec3f(0.32f, 0.32f, 1.00f), 1.0f, 0.8f)));
	spheres.push_back(unique_ptr<Shape>(new Sphere(Vec3f(-15.0f, 0.0f, -50.0f), 5, Vec3f(0.32f, 0.32f, 1.00f), 1.0f, 0.8f)));

	vector<Light> lights;

	Matrix44f pointLightMatrix1;
	pointLightMatrix1[3][1] = 15.0f;
	pointLightMatrix1[3][2] = -30.0f;
	Light pointLight(pointLightMatrix1, Vec3f(1.0f), 1.0f);

	Matrix44f pointLightMatrix2;
	pointLightMatrix2[3][0] = -10.0f;
	pointLightMatrix2[3][1] = 20.0f;
	pointLightMatrix2[3][2] = -20.0f;
	Light pointLight2(pointLightMatrix2, Vec3f(1.0f), 1.0f);

	lights.push_back(pointLight);
	lights.push_back(pointLight2);

	auto startTimer = chrono::high_resolution_clock::now();
	renderScene(spheres, lights, settings);
	auto endTimer = chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, milli>(startTimer - endTimer).count();
	cout << "Passed time:  " << passedTime / 1000 << "s" << std::endl;
	return 0;
}