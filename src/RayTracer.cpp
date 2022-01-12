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

	color = obj->lighting(lightPos, -ray.dir, ray.hit); // Object's lighting
	glm::vec3 lightVec = lightPos - ray.hit; // Vector from the point of intersection to the light source
	Ray shadowRay(ray.hit, lightVec); // Shadow ray at the point of intersection
	shadowRay.closestPt(sceneObjects); // Closest point of intersection on the shadow ray

	if (shadowRay.index > -1) {
		color = 0.2f * obj->getColor(); // 0.2 = ambient scale factor 
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

	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 0.0, -90.0), 15.0);
	sphere1->setColor(glm::vec3(0, 0, 1));
	sphere1->setSpecularity(false);
	sceneObjects.push_back(sphere1); 

	SceneObject* sphere2 = new Sphere(glm::vec3(5, -10, -60), 5);
	sphere2->setColor(glm::vec3(0, 1, 0));
	sphere2->setShininess(5);
	sceneObjects.push_back(sphere2);

	SceneObject* sphere3 = new Sphere(glm::vec3(5, 5, -70), 4);
	sphere3->setColor(glm::vec3(1, 0, 0));
	sceneObjects.push_back(sphere3);

	SceneObject* sphere4 = new Sphere(glm::vec3(10, 10, -60), 3);
	sphere4->setColor(glm::vec3(1, 1, 0));
	sceneObjects.push_back(sphere4);
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
