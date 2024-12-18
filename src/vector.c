#include "vector.h"
#include <math.h>

Vec3 add(Vec3 v1, Vec3 v2){
    Vec3 addResult;
    
    addResult.x = v1.x + v2.x;
    addResult.y = v1.y + v2.y;
    addResult.z = v1.z + v2.z;

    return addResult;
}

Vec3 subtract(Vec3 v1, Vec3 v2){
    Vec3 subResult;
    
    subResult.x = v1.x - v2.x;
    subResult.y = v1.y - v2.y;
    subResult.z = v1.z - v2.z;

    return subResult;

}

Vec3 scalarMultiply(float s, Vec3 v){
    Vec3 multResult;

    multResult.x = s * v.x;
    multResult.y = s * v.y;
    multResult.z = s * v.z;

    return multResult;
}

Vec3 scalarDivide(Vec3 v, float d){
    Vec3 divResult;

    divResult.x = v.x / d;
    divResult.y = v.y / d;
    divResult.z = v.z / d;

    return divResult;
}

Vec3 normalize(Vec3 v){
    Vec3 normResult;

    float vxSq, vySq, vzSq;

    vxSq = (v.x)*(v.x);
    vySq = (v.y)*(v.y);
    vzSq = (v.z)*(v.z);

    float vectorLen = sqrtf(vxSq + vySq + vzSq);

    if (vectorLen == 0) {
        normResult.x = 0;
        normResult.y = 0;
        normResult.z = 0;
    } 
    else{
        normResult.x = v.x / vectorLen;
        normResult.y = v.y / vectorLen;
        normResult.z = v.z / vectorLen;
    }

    return normResult;
}

float dot(Vec3 v1, Vec3 v2){
    float v1u1, v2u2, v3u3;

    v1u1 = v1.x * v2.x;
    v2u2 = v1.y * v2.y;
    v3u3 = v1.z * v2.z;

    float dotProduct = v1u1 + v2u2 + v3u3;

    return dotProduct;
}

float length(Vec3 v){
    float vxSq, vySq, vzSq;

    vxSq = (v.x)*(v.x);
    vySq = (v.y)*(v.y);
    vzSq = (v.z)*(v.z);

    float vectorLen = sqrtf(vxSq + vySq + vzSq);

    return vectorLen;
}

float length2(Vec3 v){
    float vxSq = v.x * v.x;
    float vySq = v.y * v.y;
    float vzSq = v.z * v.z;

    float vectorLenSquared = vxSq + vySq + vzSq;

    return vectorLenSquared;
}

float distance(Vec3 v1, Vec3 v2){
    float dx, dy, dz;

    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
    dz = v2.z - v1.z;

    float distanaceBtwnVectors = sqrtf((dx*dx)+(dy*dy)+(dz*dz));

    return distanaceBtwnVectors;
}

float distance2(Vec3 v1, Vec3 v2){
    float dx, dy, dz;

    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
    dz = v2.z - v1.z;

    float distanaceBtwnVectorsSquared = (dx*dx)+(dy*dy)+(dz*dz);

    return distanaceBtwnVectorsSquared;
}


