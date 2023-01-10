

float rand(uint2 *state)
{
    uint A=4294883355U;
    uint x=state->x, c=state->y;
    uint res=x^c;                     // Calculate the result
    uint hi=mul_hi(x, A);              // Step the RNG
    x=x*A+c;
    c=hi+(x<c);
    state->x = x;
    state->y = c;
    return (float)res / 4294967295.0f;           // Return the next result
}

struct Ray
{
    float3 P, D;
};

__kernel void SetupRays(__global struct Ray *rays, float3 cameraFrom, float3 cameraLF, float3 cameraRight, float3 cameraUp,
                        __global int *randNum)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);
    uint index = y*width+x;
    uint2 state = randNum[index];

    float u = (x + rand(&state)) / width;
    float v = 1 - (float)(y + rand(&state)) / height;

    float3 to = cameraLF + cameraRight * u + cameraUp * v;

    rays[index].P = cameraFrom;
    rays[index].D = to;
}


__kernel void RayIntersect(__global struct Ray *rays, __global uchar *rayValid, __global const float4 *spheres,
                           __global const int *sphereMate, __global const int *mateType,
                           __global float *intersectT, __global uchar *intersectHit, int depth)
{
    uint rayIndex = get_global_id(0);
    uint sphereIndex = get_global_id(1);
    uint index = sphereIndex * get_global_size(0) + rayIndex;

    if (rayValid[rayIndex] > 0)
        return;

    if (depth == 0 && mateType[sphereMate[sphereIndex]] == 2)
    {
        intersectHit[index] = 1;
        intersectT[index] = 1000000000000;
        return;
    }

    float3 rayP = rays[rayIndex].P;
    float3 rayD = rays[rayIndex].D;
    float sphereR = spheres[sphereIndex].w;

    float3 oc = rayP - spheres[sphereIndex].xyz;
    float a = dot(rayD, rayD);
    float b = dot(rayD, oc) * 2;
    float c = dot(oc, oc) - sphereR * sphereR;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0)
    {
        intersectHit[index] = 1;
        return;
    }

    discriminant = sqrt(discriminant);

    intersectT[index] = (-b - discriminant) / (2*a);
    intersectHit[index] = 0;

}

__kernel void RayIntersectObj(__global const float *intersectT, __global const uchar *intersectHit, __global uchar *rayValid, __global int *intersectObj)
{
    uint rayIndex = get_global_id(0);
    uint sphereIndex = get_global_id(1);

    // hopfully this is the max float
    // the exponent is supposed to be 38
    // but I'll put it at 37 just to be safe
    float fMin = 3.402823466f * pow(10.0f, 37.0f);
    int minIndex = -1;

    if (rayValid[rayIndex] > 0)
        return;

    for (int i = 0; i < get_global_size(1); i++)
    {
        int index = i * get_global_size(0) + rayIndex;
        if (intersectHit[index] <= 0 &&
            intersectT[index] > 0.0001 &&
            intersectT[index] < fMin)
        {
            fMin = intersectT[index];
            minIndex = i;
        }
    }

    intersectObj[rayIndex] = minIndex;
    if (minIndex < 0)
        rayValid[rayIndex] = 1;
}

__kernel void RayColor(__global uchar *rayValid, __global const int *intersectObj,
                       __global const int *sphereMate,
                       __global const float4 *materials, __global const int *mateType,
                       __global float3 *colorMul)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);
    uint index = y * width + x;

    if (rayValid[index] > 0)
        return;

    int rayHitMate = sphereMate[intersectObj[index]];
    int rayHitMateType = mateType[rayHitMate];

    if (rayHitMateType == 2)
    {
        //rayValid[index] = 2;
        return;
    }
    //colorAdd[index*3+0] = colorAdd[index*3+0] + materials[rayHitMate*4+0] * colorMul[index*3+0];
    //colorAdd[index*3+1] = colorAdd[index*3+1] + materials[rayHitMate*4+1] * colorMul[index*3+1];
    //colorAdd[index*3+2] = colorAdd[index*3+2] + materials[rayHitMate*4+2] * colorMul[index*3+2];


    colorMul[index] = materials[rayHitMate].xyz * colorMul[index];
}

float3 RandDir(uint2 *state)
{
    float3 p;
    do
    {
        p = (float3)(rand(state), rand(state), rand(state)) * 2.0f - (float3)(1, 1, 1);
    } while (dot(p, p) > 1);
    return p;
}

float3 Reflect(float3 D, float3 N)
{
    return D - N * (2*dot(N,D));
}

__kernel void RayScatter(__global struct Ray *ray, __global uchar *rayValid,
                         __global const int *intersectObj, __global const float *intersectT,
                         __global const float4 *spheres, __global const int *sphereMates,
                         __global const float4 *materials, __global const int *mateType,
                         __global char *depthMap,
                         __global uint2 *randNum, int depth)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);
    uint index = y * width + x;

    if (rayValid[index] > 0)
    {
        if (rayValid[index] == 1)
            depthMap[index] = depth;
        rayValid[index] = 2;
        return;
    }

    uint2 state = randNum[index];

    int obj = intersectObj[index];
    int mate = sphereMates[obj];
    int rayHitMateType = mateType[mate];

    float3 rP = ray[index].P;
    float3 rD = ray[index].D;
    float3 P = rP + rD * intersectT[obj * width * height + index];
    float3 N = P - spheres[obj].xyz;
    N = normalize(N);

    // metal
    if (rayHitMateType == 1)
    {
        float3 scatter = Reflect(rD, N) + RandDir(&state) * materials[mate].w;

        ray[index].P = P;
        ray[index].D = scatter;
    }
    // diffuse
    if (rayHitMateType == 0)
    {
        float3 scatter = RandDir(&state) + N;

        ray[index].P = P;
        ray[index].D = scatter;
    }

    randNum[index] = state;
}




__kernel void RayToColor(__global const struct Ray *ray, __global const int *intersectObj,
                         __global const int *sphereMates,
                         __global const float4 *materials, __global const int *mateType,
                         __global float3 *colors, __global float3 *colorMul,
                         __global const char *depthMap,
                         float origMul, float addMul)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);
    uint index = y * width + x;


    float3 color;
    if (intersectObj[index] < 0)
        color = (float3)(0, 0, 0);//mix((float3)(0, 0, 0), (float3)(0.1f, 0.1f, 0.1f), ray[index].D.y * 0.5f + 0.5f);
    else
    {
        int mate = sphereMates[intersectObj[index]];
        int rayHitMateType = mateType[mate];

        if (rayHitMateType == 2)
        {
            if (depthMap[index] == 0)
                color = min(materials[mate].xyz, (float3)(1, 1, 1)) * colorMul[index];
            else
                color = materials[mate].xyz * colorMul[index];
        }
        else
            color = (float3)(0, 0, 0);
    }

    float3 c = colors[index] * origMul + color * addMul;

    c = min(c, (float3)(1, 1 ,1));
    c = max(c, (float3)(0, 0, 0));

    colors[index] = c;
}

__kernel void ColorToBits(__global const float3 *colors, __global int *colorBits)
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);
    uint width = get_global_size(0);
    uint height = get_global_size(1);
    uint index = y * width + x;
    
    float3 c = sqrt(colors[index]) * 255;

    colorBits[index] = (( (int)c.x << 8 ) + (int)c.y << 8) + (int)c.z;
}