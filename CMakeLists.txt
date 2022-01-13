cmake_minimum_required(VERSION 2.8)

project(scene)

add_executable(RayTracer.out src/RayTracer.cpp src/Ray.cpp src/SceneObject.cpp
     src/Sphere.cpp src/Cone.cpp src/Cylinder.cpp src/Plane.cpp src/TextureBMP.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} )

target_link_libraries( RayTracer.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLM_LIBRARY} )

