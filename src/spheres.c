#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "spheres.h"
#include "vector.h"


void worldInit(World *world){
    world->capacity = 10;  //Here we set the initial capacity of the world to 10
    world->size = 0;       //we set the size of the world to 0 as we have no spheres at the start
    world->spheres = malloc(world->capacity * sizeof(Sphere*)); //we allocate memory for the array of Sphere pointers
    
    if (world->spheres == NULL) {
        perror("Failed to allocate memory for spheres");
        exit(EXIT_FAILURE);
    }
}

void freeWorld(World *world){
    //first we free each allocated Sphere
    for (int i = 0; i < world->size; i++) {
        if (world->spheres[i] != NULL) {
            free(world->spheres[i]);
            world->spheres[i] = NULL;  //we avoid dangling pointers
        }
    }

    // Now we free the array of Sphere pointers itself
    free(world->spheres);
    world->spheres = NULL;  //we avoid dangling pointers
    
    world->size = 0;
    world->capacity = 0;
}

void resizeWorld(World *world) {
    int newCapacity = world->capacity * 2;  //we double the capacity
    Sphere **newSpheres = realloc(world->spheres, newCapacity * sizeof(Sphere*)); /*here we are attempting to extend the memory of the 
                                                                                    array spheres by using realloc to accomadate for 
                                                                                    array newSpheres which is double the array sphere */
    if (newSpheres == NULL) {
        perror("Failed to reallocate memory for spheres");
        exit(EXIT_FAILURE);
    }
    world->spheres = newSpheres; //here we update the spheres pointer in world to the new memory location.
    world->capacity = newCapacity; // here we update capacity to show the new size of the array.
}

void addSphere(World *world, Sphere *sphere) {
    //here we check if the current capacity needs to be increased
    if (world->size >= world->capacity) {
        resizeWorld(world);  //if yest, then we resize the array
    }

    //here we add the new sphere at the end of the array and increment the size
    world->spheres[world->size] = sphere;
    world->size++;
}

Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    //we allocate memory for the new Sphere
    Sphere *newSphere = malloc(sizeof(Sphere));
    if (newSphere == NULL) {
        perror("Failed to allocate memory for new Sphere");
        exit(EXIT_FAILURE);
    }

    //once the spehre is created we initialize the properties of the sphere
    newSphere->r = radius;
    newSphere->pos = position;
    newSphere->color = color;

    return newSphere;
}

float calculateDiscriminant(float a, float b, float c){
    return b * b - 4 * a * c;
}

int solveQuadratic(float a, float b, float c, float *t1, float *t2){
    float discriminant = calculateDiscriminant(a, b, c);

    if (discriminant < 0){
        *t1 = *t2 = -1;
        return 0;
    }
    *t1 = (-b - sqrt(discriminant)) / (2 * a);
    *t2 = (-b + sqrt(discriminant)) / (2 * a);
    return 1;
}

int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t){
    Vec3 spherePos = sphere->pos;
    float sphereRadius = sphere->r;

    // Get a, b, c for the quadratic formula
    Vec3 V = subtract(rayPos,spherePos);
    float a = dot(rayDir, rayDir);
    float b = 2*dot(rayDir, V);
    float c = dot(V, V) - sphereRadius*sphereRadius;

    float t1, t2;

    if (!solveQuadratic(a, b, c, &t1, &t2)) {
        return 0; // No real roots, the ray does not intersect the sphere
    }

    if (t1 < 0 && t2 < 0){ // Both intersections are behind the ray
        return 0;
    }

    if (t1 < 0){
        *t = t2; // t1 is behind the camera, we use t2
    }
    else if (t2 < 0){
        *t = t1; // t2 is behind the camera, we use t1
    }
    else {
        *t = fmin(t1, t2); //choose the closest one
    }

    return 1;
    // Vec3 intersectionPoint = add(rayPos, scalarMultiply(*t,rayDir));
    // Vec3 normal = subtract(intersectionPoint,spherePos);

}

