/*
 * Copyright (c) 2022 Jack Brokenshire.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Ray.h"

// Finds the closest point of intersection of the current ray with scene objects.
void Ray::closestPt(std::vector<SceneObject*> &sceneObjects)
{
	glm::vec3 point(0,0,0);
	float tmin = 1.e+6;
    for(int i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(p0, dir);
		if(t > 0) // Intersects the object.
		{
			point = p0 + dir*t;
			if(t < tmin)
			{
				hit = point;
				index = i;
				dist = t;
				tmin = t;
			}
		}
	}
}

