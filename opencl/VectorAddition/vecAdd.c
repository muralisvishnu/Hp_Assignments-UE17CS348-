#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char ** argv) {
	int SIZE = 1024;
	float *A = (float*)malloc(sizeof(float)*SIZE);
	float *B = (float*)malloc(sizeof(float)*SIZE);
	float *C = (float*)malloc(sizeof(float)*SIZE);
	int i = 0;
	for (i=0; i<SIZE; ++i) {
		A[i] = i+1;
		B[i] = (i+1)*2;
	}
	FILE *kernelFile;
	char *kernelSource;
	size_t kernelSize;
	kernelFile = fopen("vecAddKernel.cl", "r");
	kernelSource = (char*)malloc(0x100000);
	kernelSize = fread(kernelSource, 1, 0x100000, kernelFile);
	fclose(kernelFile);
	cl_platform_id platformId = NULL;
	cl_device_id deviceID = NULL;
	cl_uint retNumDevices;
	cl_uint retNumPlatforms;
	cl_int ret = clGetPlatformIDs(1, &platformId, &retNumPlatforms);
	ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, &deviceID, &retNumDevices);
	cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL,  &ret);
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceID, 0, &ret);
	cl_mem aMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(float), NULL, &ret);cl_mem bMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(float), NULL, &ret);cl_mem cMemObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SIZE * sizeof(float), NULL, &ret);
	ret = clEnqueueWriteBuffer(commandQueue, aMemObj, CL_TRUE, 0, SIZE * sizeof(float), A, 0, NULL, NULL);ret = clEnqueueWriteBuffer(commandQueue, bMemObj, CL_TRUE, 0, SIZE * sizeof(float), B, 0, NULL, NULL);
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, (const size_t *)&kernelSize, &ret);	
	ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);cl_kernel kernel = clCreateKernel(program, "addVectors", &ret);
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&aMemObj);	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bMemObj);ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cMemObj);	
	size_t globalItemSize = SIZE;size_t localItemSize = 64;
	ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalItemSize, &localItemSize, 0, NULL, NULL);	ret = clEnqueueReadBuffer(commandQueue, cMemObj, CL_TRUE, 0, SIZE * sizeof(float), C, 0, NULL, NULL);
	for (i=0; i<SIZE; ++i) {
		printf("%f + %f = %f\n", A[i], B[i], C[i]);
	}
	if (i == SIZE) {
		printf("Done\n");
	}
	ret = clFlush(commandQueue);ret = clFinish(commandQueue);ret = clReleaseCommandQueue(commandQueue);ret = clReleaseKernel(kernel);ret = clReleaseProgram(program);ret = clReleaseMemObject(aMemObj);ret = clReleaseMemObject(bMemObj);ret = clReleaseMemObject(cMemObj);ret = clReleaseContext(context);
	free(A);free(B);free(C);
	return 0;
	}