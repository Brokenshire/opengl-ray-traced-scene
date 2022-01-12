/*
 * Copyright (c) 2022 Jack Brokenshire.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "Cone.h"
#include "Cylinder.h"
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX = WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX = HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;


// Computes the colour value obtained by tracing a ray and finding its 
// closest point of intersection with objects in the scene.
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);	// Background colour = (0,0,0)
	glm::vec3 lightPos(10, 40, -3); // Light's position
	glm::vec3 color(0);
	SceneObject* obj;

	ray.closestPt(sceneObjects); // Compare the ray with all objects in the scene
	if (ray.index == -1) return backgroundCol; // No intersection
	obj = sceneObjects[ray.index]; // Object on which the closest point of intersection is found

	if (ray.index == 0) {
		// Chequered pattern
		int stripeWidth = 5;
		int iz = (ray.hit.z + 100) / stripeWidth;
		int ix = (ray.hit.x + 100) / stripeWidth;
		int k = iz % 2; //2 colors
		int j = ix % 2;
		if ((k && j) || (!k && !j)) {
			color = glm::vec3(0, 1, 0);
		}
		else {
			color = glm::vec3(1, 1, 0.5);
		}
		obj->setColor(color);
	}

	color = obj->lighting(lightPos, -ray.dir, ray.hit); // Object's lighting
	glm::vec3 lightVec = lightPos - ray.hit; // Vector from the point of intersection to the light source
	Ray shadowRay(ray.hit, lightVec); // Shadow ray at the point of intersection
	shadowRay.closestPt(sceneObjects); // Closest point of intersection on the shadow ray

	if (shadowRay.index > -1) {
		color = 0.2f * obj->getColor(); // 0.2 = ambient scale factor 
	}

	if (obj->isReflective() && step < MAX_STEPS) {
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
		color = color + (rho * reflectedColor);
	}

	if (obj->isTransparent() && step < MAX_STEPS) {
		float tho = obj->getTransparencyCoeff();
		color = color * (1 - tho);
	}

	if (obj->isRefractive() && step < MAX_STEPS)
	{
		float rho = obj->getRefractionCoeff();
		float refractiveIndex = obj->getRefractiveIndex();
		float eta = 1 / refractiveIndex;
		glm::vec3 n = obj->normal(ray.hit);
		glm::vec3 g = glm::refract(ray.dir, n, eta);
		Ray refrRay(ray.hit, g);
		refrRay.closestPt(sceneObjects);
		glm::vec3 m = obj->normal(refrRay.hit);
		glm::vec3 h = glm::refract(g, -m, 1.0f / eta);
		Ray r(refrRay.hit, h);
		glm::vec3 refractedColor = trace(r, step + 1);
		color = color + (rho * refractedColor);
	}

	return color;
}


void display()
{
	float xp, yp; // grid point
	float cellX = (XMAX - XMIN) / NUMDIV; // cell width
	float cellY = (YMAX - YMIN) / NUMDIV; // cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS); // Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++) // Scan every cell of the image plane
	{
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j * cellY;

			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST); // Direction of the primary ray

			Ray ray = Ray(eye, dir);

			glm::vec3 col = trace(ray, 1); // Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp); // Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

	glEnd();
	glFlush();
}


// Creates scene objects and add them to the list of scene objects. Also, initializes the OpenGL 
// orthographc projection matrix for drawing the the ray traced image.
void initialize()
{
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

	glClearColor(0, 0, 0, 1);

	Plane* plane = new Plane(glm::vec3(-20., -15, -40), //Point A
		glm::vec3(20., -15, -40), //Point B
		glm::vec3(20., -15, -200), //Point C
		glm::vec3(-20., -15, -200)); //Point D
	plane->setColor(glm::vec3(0.8, 0.8, 0));
	plane->setSpecularity(false);
	sceneObjects.push_back(plane);

	Sphere* sphere1 = new Sphere(glm::vec3(-12.0, 0.0, -110.0), 15.0);
	sphere1->setColor(glm::vec3(1, 0, 0));  
	sphere1->setReflectivity(true, 0.8); 
	sceneObjects.push_back(sphere1);		

	Sphere* sphere2 = new Sphere(glm::vec3(8.0, 8.0, -70.0), 3.0);
	sphere2->setColor(glm::vec3(0, 0, 1));
	sphere2->setTransparency(true, 0.3);
	sphere2->setRefractivity(true, 0.8, 1.01);
	sceneObjects.push_back(sphere2);

	Sphere* sphere3 = new Sphere(glm::vec3(13.0, -2.0, -70.0), 4.0);
	sceneObjects.push_back(sphere3);	

	Sphere* sphere4 = new Sphere(glm::vec3(-8.0, 5.0, -70), 3.0);
	sphere4->setColor(glm::vec3(0, 0, 1));
	sphere4->setTransparency(true, 0.3);
	sphere4->setRefractivity(true, 0.8, 1.01);
	sceneObjects.push_back(sphere4);

	Cylinder* cylinder = new Cylinder(glm::vec3(13.0, -15.0, -70.0), 3.0, 10.0);
	cylinder->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(cylinder);

	Cone* cone = new Cone(glm::vec3(-8.0, -15.0, -70.0), 4.0, 12.0);
	cone->setColor(glm::vec3(0.62, 0.12, 0.94));
	sceneObjects.push_back(cone);

	Cone* cone2 = new Cone(glm::vec3(8.0, -10.0, -100.0), 6.0, 16.0);
	cone2->setColor(glm::vec3(0, 1, 0));
	sceneObjects.push_back(cone2);

	Plane* wall = new Plane(glm::vec3(-50., -15, -200),
		glm::vec3(50., -15, -200),
		glm::vec3(50., 50, -200),
		glm::vec3(-50., 50, -200));
	wall->setColor(glm::vec3(0.95, 0.95, 0.95));
	wall->setSpecularity(false);
	sceneObjects.push_back(wall);
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Raytracing");

	glutDisplayFunc(display);
	initialize();

	glutMainLoop();
	return 0;
}
