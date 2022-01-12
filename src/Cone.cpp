/*
 * Copyright (c) 2022 Jack Brokenshire.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Cone.h"
#include <math.h>

float Cone::intersect(glm::vec3 p0, glm::vec3 dir)
{
    glm::vec3 vdif = p0 - center;
    float ycoord = height - p0.y + center.y;
    float tangent = (radius / height) * (radius / height);
    float a = (dir.x * dir.x) + (dir.z * dir.z) - (tangent * (dir.y * dir.y));
    float b = 2 * (vdif.x * dir.x + vdif.z * dir.z + tangent * ycoord * dir.y);
    float c = (vdif.x * vdif.x) + (vdif.z * vdif.z) - (tangent * (ycoord * ycoord));
    float delta = b * b - (4 * a * c);

    if (delta < 0.0) return -1.0; // includes zero and negative values

    if (fabs(delta) < 0.001) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);

    float ypos = p0.y + dir.y * t1;
    if ((ypos >= center.y) && (ypos <= center.y + height)) {
        return t1;
    }
    else {
        float ypos = p0.y + dir.y * t2;
        if ((ypos >= center.y) && (ypos <= center.y + height)) {
            return t2;
        }
        else {
            return -1.0;
        }
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 vdif = p - center;
    float r = sqrt(vdif.x * vdif.x + vdif.z * vdif.z);
    glm::vec3 n = glm::vec3(vdif.x, r * (radius / height), vdif.z);
    n = glm::normalize(n);
    return n;
}
