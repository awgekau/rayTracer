#include "vector.h"
#include "spheres.h"
#include "color.h"


//here we define camera structure
typedef struct {
    Vec3 position;  //position of the camera
    Vec3 direction; //should point towards -z axis
} Camera;

//here we define viewport structure
typedef struct {
    float width; //width of the viewport
    float height; //height of the viewport
    float focalLength; //distance from camera to middle of the viewport
} Viewport;

//here we define the light source
typedef struct {
    Vec3 position; //position of the light source
    float brightness; //brightness of the light
} Light;


// Global variables for camera and viewport
Camera camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}; //Camera is always positioned at 0,0,0 on the grit and faces the -z axis, thereore focallength is -ve
Viewport viewport; 
Vec3 backgroundColor;
int numColors;  //number of colors provided
Light light;





void initializeViewport(float viewportWidth, float viewportHeight, float focalLength);
Vec3 calculateRayDirection(float px, float py, int imageWidth, int imageHeight);
Vec3 traceRay(Vec3 rayOrigin, Vec3 rayDirection, World world, Light light);
int isInShadow(Vec3 point, Light light, World world);
Vec3 calculateColor(Vec3 intersectionPoint, Vec3 normal, Vec3 sphereColor, Light light);
void printVec3(FILE* outputFile, Vec3 v);