/* Purpose: Assignment 1
 * Author: Kaustubh Khilari
 * Date: November 6th, 2024
 * Persons discussed w/: -
 * References: https://stackoverflow.com/questions/3723846/convert-from-hex-color-to-rgb-struct-in-c
 */

#include "assg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void initializeViewport(float viewportWidth, float viewportHeight, float focalLength) { //this is a function to initialize viewport
    viewport.focalLength = focalLength;
    viewport.width = viewportWidth;
    viewport.height = viewportHeight;
}

Vec3 calculateRayDirection(float px, float py, int imageWidth, int imageHeight) {
    //here we are calculating the size of one pixel in viewport
    float pixelWidth = viewport.width / imageWidth;
    float pixelHeight = viewport.height / imageHeight;

    //here we are calculating the position of the pixel in viewport
    float vpX = (px + 0.5f) * pixelWidth - (viewport.width / 2.0f);
    float vpY = (imageHeight - py - 0.5f) * pixelHeight - (viewport.height / 2.0f);
    float vpZ = -viewport.focalLength; // The viewport is along the negative Z-axis

    //we create the position vector of the pixel on the viewport
    Vec3 pixelPosition = {vpX, vpY, vpZ}; //we invert Y to account from screen to viewport

    //We calculate the ray direction by subtracting the camera position from the pixel position
    Vec3 rayDirection = subtract(pixelPosition, camera.position);

    //Here we normalize the direction vector
    rayDirection = normalize(rayDirection);

    return rayDirection;
}


Vec3 traceRay(Vec3 rayOrigin, Vec3 rayDirection, World world, Light light) {
    float nearestT = INFINITY; //here we initialize the nearest intersection distance as infinity
    Sphere *nearestSphere = NULL;// this is the pointer to keep track of the closest intersected sphere
    Vec3 intersectionPoint, normal;

    // we check for the closest intersection with any sphere
    for (int i = 0; i < world.size; i++) {
        float t;
        if (doesIntersect(world.spheres[i], rayOrigin, rayDirection, &t) && t < nearestT && t > 0) {
            nearestT = t;
            nearestSphere = world.spheres[i];
            intersectionPoint = add(rayOrigin, scalarMultiply(t,rayDirection));
            normal = subtract(intersectionPoint, nearestSphere->pos);
            normal = normalize(normal);
        }
    }

    if (nearestSphere) {
        Vec3 surfaceLightingColor = calculateColor(intersectionPoint, normal, nearestSphere->color, light);
        
        Vec3 shiftedIntersectionPoint = add(intersectionPoint, scalarMultiply(0.001f,normal)); //we do this to avoid self-shadowing
        if (isInShadow(shiftedIntersectionPoint, light, world)) {
            float shadowFactor = 0.1f;
            surfaceLightingColor = scalarMultiply(shadowFactor, surfaceLightingColor); // we scale down the color by shadow factor
        }
        return surfaceLightingColor;
    }
    return backgroundColor; 
}


int isInShadow(Vec3 point, Light light, World world) {
    Vec3 toLight = subtract(light.position, point); // here we calculate the vector from the point to the light source
    float distanceToLight = length(toLight); 
    toLight = normalize(toLight);
    //here we check for intersections with any sphere along the direction to the light
    for (int i = 0; i < world.size; i++) {
        float t;
        if (doesIntersect(world.spheres[i], point, toLight, &t) && t > 0 && t < distanceToLight) { // If there's an intersection between the point and the light, the point is in shadow
            return 1;
        }
    }
    return 0;
}


Vec3 calculateColor(Vec3 intersectionPoint, Vec3 normal, Vec3 sphereColor, Light light) {
    Vec3 lightDir = subtract(light.position, intersectionPoint);
    lightDir = normalize(lightDir);
    float dotProd = fmax(dot(lightDir, normal), 0.0);
    float distanceSq = distance2(light.position, intersectionPoint);
    float Io = light.brightness * (dotProd / distanceSq);

    float intensity = fmin(1.0, Io); 

    
    return (Vec3){
        sphereColor.x * intensity,
        sphereColor.y * intensity,
        sphereColor.z * intensity
    };
}

void printVec3(FILE* outputFile, Vec3 v) { //we use this function to write the output into our output file 
    fprintf(outputFile, "(%.1f, %.1f, %.1f)", v.x, v.y, v.z);
}

int main(int argc, char *argv[]) {
    
    if (argc < 3) { //we check if the input and output file names are provided and if they are not we exit the program
        fprintf(stderr, "Usage: %s <inputFile> <outputFile>\n", argv[0]);
        return EXIT_FAILURE;
    }


    FILE *inputFile = fopen(argv[1], "r"); //here we open the inputfile in read mode
    if (!inputFile) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *outputFile = fopen(argv[2], "w"); //here we open the outputfile in write mode
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    //here we are initializing the world
    World world;
    worldInit(&world);

    int imageWidth, imageHeight;
    float viewportHeight, focalLength;

    //here we are reading all the values that are provided to us in the inputfile
    fscanf(inputFile, "%d %d", &imageWidth, &imageHeight);
    fscanf(inputFile, "%f", &viewportHeight);
    fscanf(inputFile, "%f", &focalLength);


    fscanf(inputFile, "%f %f %f %f", &light.position.x, &light.position.y, &light.position.z, &light.brightness);


    fscanf(inputFile, "%d", &numColors);
    unsigned int *colorsinHex = malloc(numColors * sizeof(unsigned int));
    if (colorsinHex == NULL) {
        perror("Failed to allocate memory for colors");
        fclose(inputFile);
        fclose(outputFile);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < numColors; i++) {
        /*here we use this forloop to convert each and every color given to us in hexadecimal format to rgb format*/
        unsigned int hexColor;
        fscanf(inputFile, "%x", &hexColor); //we read color in hexadecimal format
        // colors[i] = (Vec3){1.0f, 1.0f, 1.0f};
        colorsinHex[i] = hexColor;
    }
    qsort(colorsinHex, numColors, sizeof(unsigned int), compareColor);

    Vec3 *colors = malloc(numColors * sizeof(Vec3));
    if (colors == NULL) {
        perror("Failed to allocate memory for colors (Vec3)");
        free(colorsinHex);
        fclose(inputFile);
        fclose(outputFile);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < numColors; i++) {
        colors[i] = unpackRGB(colorsinHex[i]);
    }
    free(colorsinHex);

    int backgroundColorIndex;
    fscanf(inputFile, "%d", &backgroundColorIndex);
    if (backgroundColorIndex < 0 || backgroundColorIndex >= numColors) {
        fprintf(stderr, "Invalid background color index. Using default.\n");
        backgroundColorIndex = 0;
    }
    // backgroundColor = (Vec3){0.0f, 0.0f, 0.0f};
    backgroundColor = colors[backgroundColorIndex];


    int numSpheres;
    fscanf(inputFile, "%d", &numSpheres);
    for (int i = 0; i < numSpheres; i++) {
        //here we use this for loop to read the properties of each and every sphere in the inputfile
        float x, y, z, radius;
        int colorIndex;
        fscanf(inputFile, "%f %f %f %f %d", &x, &y, &z, &radius, &colorIndex);

        if (colorIndex < 0 || colorIndex >= numColors) {
            fprintf(stderr, "Invalid color index for sphere %d. Using default.\n", i + 1);
            colorIndex = 0;
        }

        Vec3 position = {x, y, z}; //here we create a Vec3 struct for the spheres position using the coordinates we read from the input file
        Vec3 color = colors[colorIndex]; //we get the color of the sphere from the colors array
        Sphere *sphere = createSphere(radius, position, color); //we create a new sphere with the given radius, position and color
        addSphere(&world, sphere);//and then we add this created sphere to our list of spheres in world
    }

    fclose(inputFile);


    float aspectRatio = (float)imageWidth / imageHeight; //here we find the aspect ratio of image
    float viewportWidth = viewportHeight * aspectRatio; //and we use it to adjust the width of viewport
    initializeViewport(viewportWidth, viewportHeight, focalLength); //we initialize the viewport with the width we just calculated and height and focal length that were given to us

#ifdef MS1

    backgroundColor = (Vec3){0.0f, 0.0f, 0.0f}; //we set the background color to black

    //we set all sphere colors to white 
    for (int i = 0; i < world.size; i++) {
        world.spheres[i]->color = (Vec3){1.0f, 1.0f, 1.0f};
    }


    Vec3 resultAdd = add(backgroundColor, light.position);
    Vec3 resultSubtract = subtract(backgroundColor, light.position);
    Vec3 resultScalarMult = scalarMultiply(viewportWidth, light.position);
    Vec3 resultNormalize = normalize(light.position);


    printVec3(outputFile, backgroundColor);
    fprintf(outputFile, " + ");
    printVec3(outputFile, light.position);
    fprintf(outputFile, " = ");
    printVec3(outputFile, resultAdd);
    fprintf(outputFile, "\n");

    printVec3(outputFile, backgroundColor);
    fprintf(outputFile, " - ");
    printVec3(outputFile, light.position);
    fprintf(outputFile, " = ");
    printVec3(outputFile, resultSubtract);
    fprintf(outputFile, "\n");

    fprintf(outputFile, "%.1f * ", viewportWidth);
    printVec3(outputFile, light.position);
    fprintf(outputFile, " = ");
    printVec3(outputFile, resultScalarMult);
    fprintf(outputFile, "\n");

    fprintf(outputFile, "normalize");
    printVec3(outputFile, light.position);
    fprintf(outputFile, " = ");
    printVec3(outputFile, resultNormalize);
    fprintf(outputFile, "\n");


    //we calculate for each sphere
    for (int i = 0; i < world.size; i++) {
        Sphere *sphere = world.spheres[i];

        Vec3 resultScalarDiv = scalarDivide(sphere->color, sphere->r);
        float resultDot = dot(light.position, sphere->pos);
        float resultDistance = distance(light.position, sphere->pos);
        float resultLength = length(sphere->pos);

        fprintf(outputFile, "\n");
        printVec3(outputFile, sphere->color);
        fprintf(outputFile, " / %.1f = ", sphere->r);
        printVec3(outputFile, resultScalarDiv);
        fprintf(outputFile, "\n");

        fprintf(outputFile, "dot(");
        printVec3(outputFile, light.position);
        fprintf(outputFile, ", ");
        printVec3(outputFile, sphere->pos);
        fprintf(outputFile, ") = %.1f\n", resultDot);

        fprintf(outputFile, "distance(");
        printVec3(outputFile, light.position);
        fprintf(outputFile, ", ");
        printVec3(outputFile, sphere->pos);
        fprintf(outputFile, ") = %.1f\n", resultDistance);

        fprintf(outputFile, "length");
        printVec3(outputFile, sphere->pos);
        fprintf(outputFile, " = %.1f\n", resultLength);
    }
#endif // MS1

#ifdef MS2
    backgroundColor = (Vec3){0.0f, 0.0f, 0.0f}; //we set the background color to black

    //we set all sphere colors to white 
    for (int i = 0; i < world.size; i++) {
        world.spheres[i]->color = (Vec3){1.0f, 1.0f, 1.0f};
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", imageWidth, imageHeight);
    //here we loop over each pixel starting from the top-left corner
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            //here we calculate the ray direction for pixel (x, y)
            Vec3 rayDirection = calculateRayDirection(x, y, imageWidth, imageHeight);

            //we trace the ray and get the color
            Vec3 color = traceRay(camera.position, rayDirection, world, light);


            writeColour(outputFile, color);
        }

        fprintf(outputFile, "\n");
    }
#endif

#ifdef FS
    fprintf(outputFile, "P3\n%d %d\n255\n", imageWidth, imageHeight);
    //here we loop over each pixel starting from the top-left corner
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            //we initialize the combined color to black
            Vec3 combinedColor = (Vec3){0.0f, 0.0f, 0.0f};

            //here we split each pixel into 9 smaller squares (3X3 grid) take sample of each sqaure and then avg the result
            for (int splitY = 0; splitY < 3; splitY++) {
                for (int splitX = 0; splitX < 3; splitX++) {
                    float sampleX = x + (splitX + 0.5f) / 3.0f;
                    float sampleY = y + (splitY + 0.5f) / 3.0f;

                    //here we calculate the ray direction for this split sample
                    Vec3 rayDirection = calculateRayDirection(sampleX, sampleY, imageWidth, imageHeight);

                    //we trace the ray to get the sample color
                    Vec3 sampleColor = traceRay(camera.position, rayDirection, world, light);

                    //then we combine the split sample colors
                    combinedColor = add(combinedColor, sampleColor);
                }
            }

            //finally we average the combined color over the 9 samples
            Vec3 finalColor = scalarMultiply(1.0f / 9.0f, combinedColor);

            writeColour(outputFile, finalColor);
        }

        fprintf(outputFile, "\n");
    }
#endif


    free(colors);
    freeWorld(&world);
    fclose(outputFile);

    return 0;
}