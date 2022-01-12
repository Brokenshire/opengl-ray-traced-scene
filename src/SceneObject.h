/*
 * Copyright (c) 2022 Jack Brokenshire.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef H_SOBJECT
#define H_SOBJECT
#include <glm/glm.hpp>


class SceneObject 
{
protected:
	glm::vec3 color_ = glm::vec3(1); // Material color
	bool refl_ = false; // Reflectivity: true/false
	bool refr_ = false; // Refractivity: true/false
	bool spec_ = true; // Specularity: true/false
	bool tran_ = false; // Transparency: true/false
	float reflc_ = 0.8; // Coefficient of reflection
	float refrc_ = 0.8; // Coefficient of refraction
	float tranc_ = 0.8; // Coefficient of transparency
	float refri_ = 1.0; // Refractive index
	float shin_ = 50.0; // Shininess
public:
	SceneObject() {}
    virtual float intersect(glm::vec3 p0, glm::vec3 dir) = 0;
	virtual glm::vec3 normal(glm::vec3 pos) = 0;
	virtual ~SceneObject() {}

	glm::vec3 lighting(glm::vec3 lightPos, glm::vec3 viewVec, glm::vec3 hit);
	void setColor(glm::vec3 col);
	void setReflectivity(bool flag);
	void setReflectivity(bool flag, float refl_coeff);
	void setRefractivity(bool flag);
	void setRefractivity(bool flag, float refr_coeff, float refr_indx);
	void setShininess(float shininess);
	void setSpecularity(bool flag);
	void setTransparency(bool flag);
	void setTransparency(bool flag, float tran_coeff);
	glm::vec3 getColor();
	float getReflectionCoeff();
	float getRefractionCoeff();
	float getTransparencyCoeff();
	float getRefractiveIndex();
	float getShininess();
	bool isReflective();
	bool isRefractive();
	bool isSpecular();
	bool isTransparent();
};

#endif