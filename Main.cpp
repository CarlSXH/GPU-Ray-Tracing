

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C runtime libraries
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>

#include <string>

#include "VectorF.h"

// OpenCL
#pragma comment(lib, "OpenCL.lib")
#include <CL/cl.h>


// Application global variables

HINSTANCE g_hInst = NULL;
const WCHAR g_szTitle[] = L"Ray Tracing GPU";
WCHAR g_szWindowClass[] = L"RAY_TRACING_OPENCL_GPU";

HWND g_hWnd = NULL;

int g_Width = 700;
int g_Height = 500;



// OpenCL global variables

cl_context g_ClContext = 0;
cl_command_queue g_ClCommandQueue = 0;

cl_program g_Program;
cl_kernel g_SetupRays;
cl_kernel g_RayIntersect;
cl_kernel g_RayIntersectObj;
cl_kernel g_RayColor;
cl_kernel g_RayScatter;
cl_kernel g_RayToColor;
cl_kernel g_ColorToBits;

// Ray tracing global variables

int *g_pRawColor = NULL;

cl_mem g_RayValid;
cl_mem g_Rays;

cl_mem g_Spheres;
cl_mem g_SphereMateIndex;

cl_mem g_Materials;
cl_mem g_MaterialsType;

cl_mem g_Colors;
cl_mem g_ColorBits;
cl_mem g_ColorsMul;

cl_mem g_IntersectT;
cl_mem g_IntersectHit;
cl_mem g_IntersectObj;
cl_mem g_DepthMap;

cl_mem g_RandMap;

int g_Sample = 0;
int g_RefreshCount = 0;




char *ReadFile(const char *pFileName)
{
    FILE *fp;
    char *source_str;
    fp = fopen(pFileName, "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // seek to end of file
    size_t source_size = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET);
    source_str = new char[source_size+1];
    source_size = fread(source_str, 1, source_size, fp);
    source_str[source_size] = 0;
    fclose(fp);

    return source_str;
}
void ReleaseStr(char *pStr)
{
    delete[] pStr;
}


float RandRange(const float Min = 0, const float Max = 1)
{
    return rand() * 3.0518509475997192297128208258309e-5 * (Max - Min) + Min;
}

struct cl_ray
{
    cl_float3 p;
    cl_float3 d;
};

struct Performance
{
    float SetupRays;
    float RayIntersect;
    float RayIntersectObj;
    float RayColor;
    float RayScatter;
    float RayToColor;
    float ColorToBits;
    float Transfer;
};

Performance g_Performance;
int sphereCount = 21;

void InitRayTracing()
{
    cl_int ret;
    cl_uint platformIdCount = 0;
    cl_platform_id platformID = 0;
    clGetPlatformIDs (1, &platformID, &platformIdCount);

    cl_device_id deviceID = 0;
    cl_uint deviceIDCount = 0;
    ret = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, &deviceIDCount);

    g_ClContext = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &ret);
    g_ClCommandQueue = clCreateCommandQueue(g_ClContext, deviceID, 0, &ret);

    // creating kernels
    {
        char *pKernel = ReadFile("RayTracing.cl");
        size_t sourceSize = strlen(pKernel)+1;
        g_Program = clCreateProgramWithSource(g_ClContext, 1, (const char **)(&pKernel), (const size_t *)(&sourceSize), &ret);
        ret = clBuildProgram(g_Program, 1, &deviceID, NULL, NULL, NULL);

        size_t errorSize;
        ret = clGetProgramBuildInfo(g_Program, deviceID, CL_PROGRAM_BUILD_LOG, 0, NULL, &errorSize);
        char *errorMessage = new char[errorSize+1];
        errorMessage[errorSize] = 0;
        ret = clGetProgramBuildInfo(g_Program, deviceID, CL_PROGRAM_BUILD_LOG, errorSize, errorMessage, 0);
        if (errorMessage[0] != '\n')
        {
            int n = 0;
        }
        delete[] errorMessage;

        g_SetupRays = clCreateKernel(g_Program, "SetupRays", &ret);
        g_RayIntersect = clCreateKernel(g_Program, "RayIntersect", &ret);
        g_RayIntersectObj = clCreateKernel(g_Program, "RayIntersectObj", &ret);
        g_RayColor = clCreateKernel(g_Program, "RayColor", &ret);
        g_RayScatter = clCreateKernel(g_Program, "RayScatter", &ret);
        g_RayToColor = clCreateKernel(g_Program, "RayToColor", &ret);
        g_ColorToBits = clCreateKernel(g_Program, "ColorToBits", &ret);

        ReleaseStr(pKernel);
    }

    int materialCount = 18;

    // creating memory buffers
    {
        g_RayValid = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(unsigned char), 0, 0);
        g_Rays = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(cl_ray), 0, 0);

        g_Spheres = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, sphereCount * sizeof(cl_float4), 0, 0);
        g_SphereMateIndex = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, sphereCount * sizeof(int), 0, 0);

        g_Materials = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, materialCount * (4 * sizeof(float)), 0, 0);
        g_MaterialsType = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, materialCount * sizeof(int), 0, 0);

        g_Colors = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(cl_float3), 0, 0);
        g_ColorBits = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(int), 0, 0);
        g_ColorsMul = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(cl_float3), 0, 0);

        g_IntersectT = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sphereCount * sizeof(float), 0, 0);
        g_IntersectHit = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sphereCount * sizeof(bool), 0, 0);
        g_IntersectObj = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(int), 0, 0);
        g_DepthMap = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * sizeof(char), 0, 0);

        g_RandMap = clCreateBuffer(g_ClContext, CL_MEM_READ_WRITE, g_Width * g_Height * 2 * sizeof(unsigned int), 0, 0);
    }

    // creating spheres and materials
    {
        Vector4F *spheres;
        int *sphereMatIndex;

        Vector4F *materials;
        int *materialType;

        spheres = new Vector4F[sphereCount];
        sphereMatIndex = new int[sphereCount];

        materials = new Vector4F[materialCount];
        materialType = new int[materialCount];


        int sphereI = 0;
        int sphereMatIndexI = 0;
        int materialTypeI = 0;
        int materialsI = 0;

        spheres[sphereI++] = Vector4F(0, -1000, 0, 1000);
        sphereMatIndex[sphereMatIndexI++] = 0;

        //spheres[sphereI++] = Vector4F(-1003, 0, 0, 1000);
        //sphereMatIndex[sphereMatIndexI++] = 0;

        //spheres[sphereI++] = Vector4F(0, 0, 1003, 1000);
        //sphereMatIndex[sphereMatIndexI++] = 0;

        //spheres[sphereI++] = Vector4F(0, 0, -1003, 1000);
        //sphereMatIndex[sphereMatIndexI++] = 0;



        materialType[materialTypeI++] = 0;
        materials[materialsI++] = Vector4F(0.5f, 0.5f, 0.5f, 0.0f);


        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
            {
                spheres[sphereI++] = Vector4F(i - 0.5f, 1, j - 0.5f, 0.25f);
                sphereMatIndex[sphereMatIndexI++] = 1;
            }
        materialType[materialTypeI++] = 2;
        materials[materialsI++] = Vector4F(4, 4, 4, 0);

        srand(1);

        for (int a = -2; a < 2; a++)
        {
            for (int b = -2; b < 2; b++)
            {
                spheres[sphereI++] = Vector4F(a + 0.9f*RandRange(), 0.2f, b + 0.9f*RandRange(), 0.2f);
            
            

                if (true)//RandRange() > 0.84210526315789473684210526315789f)
                {
                    materialType[materialTypeI++] = 1;
                    materials[materialsI++] = Vector4F(RandRange(0.5f, 1), RandRange(0.5f, 1), RandRange(0.5f, 1), RandRange(0, 0.5f));
                }
                else
                {
                    materialType[materialTypeI++] = 0;
                    materials[materialsI++] = Vector4F(RandRange()*RandRange(), RandRange()*RandRange(), RandRange()*RandRange(), 0);
                }
                sphereMatIndex[sphereMatIndexI++] = materialsI-1;
            }
        }

        unsigned char _true = 0;
        clEnqueueFillBuffer(g_ClCommandQueue, g_RayValid, &_true, 1, 0, g_Width * g_Height * sizeof(unsigned char), 0, NULL, NULL);

        clEnqueueWriteBuffer(g_ClCommandQueue, g_Spheres, CL_TRUE, 0, sphereCount * (4 * sizeof(float)), spheres, 0, NULL, NULL);
        clEnqueueWriteBuffer(g_ClCommandQueue, g_SphereMateIndex, CL_TRUE, 0, sphereCount * sizeof(int), sphereMatIndex, 0, NULL, NULL);

        clEnqueueWriteBuffer(g_ClCommandQueue, g_Materials, CL_TRUE, 0, materialCount * (4 * sizeof(float)), materials, 0, NULL, NULL);
        clEnqueueWriteBuffer(g_ClCommandQueue, g_MaterialsType, CL_TRUE, 0, materialCount * sizeof(int), materialType, 0, NULL, NULL);

        delete[] spheres;
        delete[] sphereMatIndex;

        delete[] materials;
        delete[] materialType;
    }

    // create random number seeds
    {
        unsigned int *RandNum = new unsigned int[g_Width * g_Height * 2];

        // 15 is because RAND_MAX = 0x7FFF, which means the rand()
        // generates number that have up to 15 bits
        for (int i = 0; i < g_Width * g_Height * 2; i++)
            RandNum[i] = (rand() << 15) + rand();

        clEnqueueWriteBuffer(g_ClCommandQueue, g_RandMap, CL_TRUE, 0, g_Width * g_Height * 2 * sizeof(unsigned int), RandNum, 0, NULL, NULL);

        delete[] RandNum;
    }

    g_pRawColor = new int[g_Width * g_Height];

    g_Performance.SetupRays = 0.0f;
    g_Performance.RayIntersect = 0.0f;
    g_Performance.RayIntersectObj = 0.0f;
    g_Performance.RayColor = 0.0f;
    g_Performance.RayScatter = 0.0f;
    g_Performance.RayToColor = 0.0f;
    g_Performance.ColorToBits = 0.0f;
    g_Performance.Transfer = 0.0f;
}
void ReleaseRayTracing()
{
    delete[] g_pRawColor;
    g_pRawColor = NULL;

    {
        clReleaseMemObject(g_RandMap);

        clReleaseMemObject(g_DepthMap);
        clReleaseMemObject(g_IntersectObj);
        clReleaseMemObject(g_IntersectHit);
        clReleaseMemObject(g_IntersectT);

        clReleaseMemObject(g_ColorsMul);
        clReleaseMemObject(g_ColorBits);
        clReleaseMemObject(g_Colors);

        clReleaseMemObject(g_MaterialsType);
        clReleaseMemObject(g_Materials);

        clReleaseMemObject(g_SphereMateIndex);
        clReleaseMemObject(g_Spheres);

        clReleaseMemObject(g_Rays);
        clReleaseMemObject(g_RayValid);
    }

    {
        clReleaseKernel(g_ColorToBits);
        clReleaseKernel(g_RayToColor);
        clReleaseKernel(g_RayScatter);
        clReleaseKernel(g_RayColor);
        clReleaseKernel(g_RayIntersectObj);
        clReleaseKernel(g_RayIntersect);
        clReleaseKernel(g_SetupRays);

        clReleaseProgram(g_Program);
    }


    clReleaseCommandQueue(g_ClCommandQueue);
    clReleaseContext(g_ClContext);
}


void Refresh(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    InvalidateRect(hWnd, &rect, TRUE);
    UpdateWindow(hWnd);
}

struct Camera
{
    Vector3F LowerLeft;
    Vector3F Right, Up;
    Vector3F Position;

    Camera() :
        Position(), LowerLeft(-1, -2, 1), Right(2, 0, 0), Up(0, 4, 0)
    {

    }
    Camera(Vector3F P, Vector3F LF, Vector3F R, Vector3F U)
    {
        Position = P;
        LowerLeft = LF;
        Right = R;
        Up = U;
    }
    Camera(Vector3F lookFrom, Vector3F lookAt, Vector3F lookUp, float fov, float aspectRatio)
    {
        float halfHeight = tan(fov*0.5f);
        float halfWidth = aspectRatio * halfHeight;
        Vector3F back = (lookFrom - lookAt).Normalize();
        Vector3F right = Vector3FCross(lookUp, back).Normalize();
        Vector3F up = Vector3FCross(back, right);

        Position = lookFrom;
        LowerLeft = lookFrom - halfWidth * right - halfHeight * up - back - Position;
        Right = 2*halfWidth * right;
        Up = 2*halfHeight * up;
    }

    Vector3F param(float u, float v)
    {
        return LowerLeft + u * Right + v * Up - Position;
    }
};

void CALLBACK TimerTick(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    cl_int ret;

    Camera camera = Camera(Vector3F(10, 10, 0),
                           Vector3F(0, 0, 0),
                           Vector3F(0, 1, 0),
                           3.141592654f / 9,
                           (float)g_Width / g_Height);



    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    float fFrequency = 1.0f / frequency.QuadPart;
    LARGE_INTEGER start, end;

    for (int ____ = 0; ____ < 1; ____++)
    {
        unsigned char _true = 0;
        clEnqueueFillBuffer(g_ClCommandQueue, g_RayValid, &_true, 1, 0, g_Width * g_Height * sizeof(unsigned char), 0, NULL, NULL);
        float one = 1;
        clEnqueueFillBuffer(g_ClCommandQueue, g_ColorsMul, &one, sizeof(float), 0, g_Width * g_Height * sizeof(cl_float3), 0, NULL, NULL);
        char _minusOne = (char)(-1);
        clEnqueueFillBuffer(g_ClCommandQueue, g_DepthMap, &_minusOne, sizeof(char), 0, g_Width * g_Height * sizeof(char), 0, NULL, NULL);



        Performance current;

        current.SetupRays       = 0.0f;
        current.RayIntersect    = 0.0f;
        current.RayIntersectObj = 0.0f;
        current.RayColor        = 0.0f;
        current.RayScatter      = 0.0f;
        current.RayToColor      = 0.0f;
        current.ColorToBits     = 0.0f;
        current.Transfer        = 0.0f;

        {
            QueryPerformanceCounter(&start);

            ret = clSetKernelArg(g_SetupRays, 0, sizeof(cl_mem), &g_Rays);
            ret = clSetKernelArg(g_SetupRays, 1, sizeof(cl_float3), &camera.Position);
            ret = clSetKernelArg(g_SetupRays, 2, sizeof(cl_float3), &camera.LowerLeft);
            ret = clSetKernelArg(g_SetupRays, 3, sizeof(cl_float3), &camera.Right);
            ret = clSetKernelArg(g_SetupRays, 4, sizeof(cl_float3), &camera.Up);
            ret = clSetKernelArg(g_SetupRays, 5, sizeof(cl_mem), &g_RandMap);

            size_t workSize[2] = {g_Width, g_Height};
            ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_SetupRays, 2, NULL, workSize, NULL, 0, NULL, NULL);

            QueryPerformanceCounter(&end);

            current.SetupRays = (end.QuadPart - start.QuadPart) * fFrequency;
        }

        int maxDepth = 5;

        for (int i = 0; i < maxDepth; i++)
        {
            {
                QueryPerformanceCounter(&start);

                ret = clSetKernelArg(g_RayIntersect, 0, sizeof(cl_mem), &g_Rays);
                ret = clSetKernelArg(g_RayIntersect, 1, sizeof(cl_mem), &g_RayValid);
                ret = clSetKernelArg(g_RayIntersect, 2, sizeof(cl_mem), &g_Spheres);
                ret = clSetKernelArg(g_RayIntersect, 3, sizeof(cl_mem), &g_SphereMateIndex);
                ret = clSetKernelArg(g_RayIntersect, 4, sizeof(cl_mem), &g_MaterialsType);
                ret = clSetKernelArg(g_RayIntersect, 5, sizeof(cl_mem), &g_IntersectT);
                ret = clSetKernelArg(g_RayIntersect, 6, sizeof(cl_mem), &g_IntersectHit);
                ret = clSetKernelArg(g_RayIntersect, 7, sizeof(int), &i);

                size_t workSize[2] = {g_Width * g_Height, sphereCount };
                ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_RayIntersect, 2, NULL, workSize, NULL, 0, NULL, NULL);

                QueryPerformanceCounter(&end);

                current.RayIntersect += (end.QuadPart - start.QuadPart) * fFrequency / maxDepth;
            }

            {
                QueryPerformanceCounter(&start);

                ret = clSetKernelArg(g_RayIntersectObj, 0, sizeof(cl_mem), &g_IntersectT);
                ret = clSetKernelArg(g_RayIntersectObj, 1, sizeof(cl_mem), &g_IntersectHit);
                ret = clSetKernelArg(g_RayIntersectObj, 2, sizeof(cl_mem), &g_RayValid);
                ret = clSetKernelArg(g_RayIntersectObj, 3, sizeof(cl_mem), &g_IntersectObj);

                size_t workSize[2] = { g_Width * g_Height, sphereCount };
                ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_RayIntersectObj, 2, NULL, workSize, NULL, 0, NULL, NULL);

                QueryPerformanceCounter(&end);

                current.RayIntersectObj += (end.QuadPart - start.QuadPart) * fFrequency / maxDepth;
            }

            {
                QueryPerformanceCounter(&start);

                ret = clSetKernelArg(g_RayColor, 0, sizeof(cl_mem), &g_RayValid);
                ret = clSetKernelArg(g_RayColor, 1, sizeof(cl_mem), &g_IntersectObj);
                ret = clSetKernelArg(g_RayColor, 2, sizeof(cl_mem), &g_SphereMateIndex);
                ret = clSetKernelArg(g_RayColor, 3, sizeof(cl_mem), &g_Materials);
                ret = clSetKernelArg(g_RayColor, 4, sizeof(cl_mem), &g_MaterialsType);
                ret = clSetKernelArg(g_RayColor, 5, sizeof(cl_mem), &g_ColorsMul);

                size_t workSize[2] = { g_Width, g_Height };
                ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_RayColor, 2, NULL, workSize, NULL, 0, NULL, NULL);

                QueryPerformanceCounter(&end);

                current.RayColor += (end.QuadPart - start.QuadPart) * fFrequency / maxDepth;
            }

            {
                QueryPerformanceCounter(&start);

                ret = clSetKernelArg(g_RayScatter, 0, sizeof(cl_mem), &g_Rays);
                ret = clSetKernelArg(g_RayScatter, 1, sizeof(cl_mem), &g_RayValid);
                ret = clSetKernelArg(g_RayScatter, 2, sizeof(cl_mem), &g_IntersectObj);
                ret = clSetKernelArg(g_RayScatter, 3, sizeof(cl_mem), &g_IntersectT);
                ret = clSetKernelArg(g_RayScatter, 4, sizeof(cl_mem), &g_Spheres);
                ret = clSetKernelArg(g_RayScatter, 5, sizeof(cl_mem), &g_SphereMateIndex);
                ret = clSetKernelArg(g_RayScatter, 6, sizeof(cl_mem), &g_Materials);
                ret = clSetKernelArg(g_RayScatter, 7, sizeof(cl_mem), &g_MaterialsType);
                ret = clSetKernelArg(g_RayScatter, 8, sizeof(cl_mem), &g_DepthMap);
                ret = clSetKernelArg(g_RayScatter, 9, sizeof(cl_mem), &g_RandMap);
                ret = clSetKernelArg(g_RayScatter, 10, sizeof(int), &i);

                size_t workSize[2] = { g_Width, g_Height };
                ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_RayScatter, 2, NULL, workSize, NULL, 0, NULL, NULL);

                QueryPerformanceCounter(&end);

                current.RayScatter += (end.QuadPart - start.QuadPart) * fFrequency / maxDepth;
            }
        }

        {
            QueryPerformanceCounter(&start);

            float origMul = (float)g_Sample / (g_Sample+1);
            float addMul = 1.0f / (g_Sample+1);
            ret = clSetKernelArg(g_RayToColor, 0, sizeof(cl_mem), &g_Rays);
            ret = clSetKernelArg(g_RayToColor, 1, sizeof(cl_mem), &g_IntersectObj);
            ret = clSetKernelArg(g_RayToColor, 2, sizeof(cl_mem), &g_SphereMateIndex);
            ret = clSetKernelArg(g_RayToColor, 3, sizeof(cl_mem), &g_Materials);
            ret = clSetKernelArg(g_RayToColor, 4, sizeof(cl_mem), &g_MaterialsType);
            ret = clSetKernelArg(g_RayToColor, 5, sizeof(cl_mem), &g_Colors);
            ret = clSetKernelArg(g_RayToColor, 6, sizeof(cl_mem), &g_ColorsMul);
            ret = clSetKernelArg(g_RayToColor, 7, sizeof(cl_mem), &g_DepthMap);
            ret = clSetKernelArg(g_RayToColor, 8, sizeof(float), &origMul);
            ret = clSetKernelArg(g_RayToColor, 9, sizeof(float), &addMul);

            size_t workSize[2] = { g_Width, g_Height };
            ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_RayToColor, 2, NULL, workSize, NULL, 0, NULL, NULL);

            QueryPerformanceCounter(&end);

            current.RayToColor = (end.QuadPart - start.QuadPart) * fFrequency;
        }




        g_Performance.SetupRays       = g_Performance.SetupRays       * (g_Sample / (float)(g_Sample+1)) + current.SetupRays       / (g_Sample+1);
        g_Performance.RayIntersect    = g_Performance.RayIntersect    * (g_Sample / (float)(g_Sample+1)) + current.RayIntersect    / (g_Sample+1);
        g_Performance.RayIntersectObj = g_Performance.RayIntersectObj * (g_Sample / (float)(g_Sample+1)) + current.RayIntersectObj / (g_Sample+1);
        g_Performance.RayColor        = g_Performance.RayColor        * (g_Sample / (float)(g_Sample+1)) + current.RayColor        / (g_Sample+1);
        g_Performance.RayScatter      = g_Performance.RayScatter      * (g_Sample / (float)(g_Sample+1)) + current.RayScatter      / (g_Sample+1);
        g_Performance.RayToColor      = g_Performance.RayToColor      * (g_Sample / (float)(g_Sample+1)) + current.RayToColor      / (g_Sample+1);

        //for (int i = 0; i < g_Width; i++)
        //{
        //    for (int j = 0; j < g_Height; j++)
        //    {
        //        int index = j*g_Width + i;
        //        index = index * 3;
        //        int r = (int)(g_pRawColorF[index+0] * 255);
        //        int g = (int)(g_pRawColorF[index+1] * 255);
        //        int b = (int)(g_pRawColorF[index+2] * 255);
        //        g_pRawColor[j * g_Width + i] = ((r << 8) + g << 8) + b;
        //    }
        //}
        //
        //for (int i = 0; i < g_Width * g_Height; i++)
        //{
        //    int nHit;
        //    clEnqueueReadBuffer(g_ClCommandQueue, g_IntersectObj, CL_TRUE, i*sizeof(int), sizeof(int), &nHit, 0, NULL, NULL);
        //    if (68840 == i)
        //    {
        //    int y = 0;
        //
        //    }
        //}
        //
        //float avgDif = 0;
        //int count = 0;
        //for (int i = 0; i < g_Width*g_Height; i++)
        //{
        //    Vector3F rayP, rayD;
        //    clEnqueueReadBuffer(g_ClCommandQueue, g_Rays, CL_TRUE, i*2*3*sizeof(float), 3 * sizeof(float), &rayP, 0, NULL, NULL);
        //    clEnqueueReadBuffer(g_ClCommandQueue, g_Rays, CL_TRUE, (i*2+1)*3*sizeof(float), 3 * sizeof(float), &rayD, 0, NULL, NULL);
        //
        //    for (int j = 0; j < 21; j++)
        //    {
        //        Vector3F sphereP = Vector3F(spheres[i].X, spheres[i].Y, spheres[i].Z);
        //        float sphereR = spheres[i].W;
        //        float outT;
        //        RaySphere(rayP, rayD, sphereP, sphereR, &outT);
        //        float computeT;
        //        clEnqueueReadBuffer(g_ClCommandQueue, g_IntersectT, CL_TRUE, (j*21 + i) * sizeof(float), sizeof(float), &computeT, 0, NULL, NULL);
        //
        //        float dif = computeT - outT;
        //        avgDif = avgDif * ((float)count / (count+1)) + dif / (count+1);
        //    }
        //}

        g_Sample += 1;
    }

    {
        QueryPerformanceCounter(&start);

        ret = clSetKernelArg(g_ColorToBits, 0, sizeof(cl_mem), &g_Colors);
        ret = clSetKernelArg(g_ColorToBits, 1, sizeof(cl_mem), &g_ColorBits);

        size_t workSize[2] = { g_Width, g_Height };
        ret = clEnqueueNDRangeKernel(g_ClCommandQueue, g_ColorToBits, 2, NULL, workSize, NULL, 0, NULL, NULL);

        QueryPerformanceCounter(&end);

        g_Performance.ColorToBits = (g_Performance.ColorToBits * g_RefreshCount + (end.QuadPart - start.QuadPart) * fFrequency) / (g_RefreshCount+1);
    }
    {
        QueryPerformanceCounter(&start);

        ret = clEnqueueReadBuffer(g_ClCommandQueue, g_ColorBits, CL_TRUE, 0, g_Width*g_Height*sizeof(int), g_pRawColor, 0, NULL, NULL);

        QueryPerformanceCounter(&end);
        g_Performance.Transfer = (g_Performance.Transfer * g_RefreshCount + (end.QuadPart - start.QuadPart) * fFrequency) / (g_RefreshCount+1);
    }

    g_RefreshCount++;
    std::wstring s = std::to_wstring(g_Sample);
    SetWindowText(g_hWnd, s.c_str());
    Refresh(g_hWnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ERASEBKGND:
        return 0;
    case WM_PAINT:
    {

        RECT rect;
        GetClientRect(hWnd, &rect);
        int width=rect.right;
        int height=rect.bottom;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HBITMAP backbuffer = CreateBitmap(g_Width, g_Height, 1, 32, g_pRawColor);
        HDC backbuffDC = CreateCompatibleDC(hdc);
        SelectObject(backbuffDC, backbuffer);

        BitBlt(hdc, 0, 0, width, height, backbuffDC, 0, 0, SRCCOPY);

        DeleteDC(backbuffDC);
        DeleteObject(backbuffer);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = g_szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance;

    HWND hWnd = CreateWindowW(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, 0, g_Width + 16, g_Height + 39, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~(WS_SIZEBOX | WS_MAXIMIZEBOX));
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    g_hWnd = hWnd;

    return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    InitRayTracing();
    SetTimer(g_hWnd, 14235, 1, TimerTick);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(g_hWnd, 14235);
    ReleaseRayTracing();

    return (int)msg.wParam;
}
