#include <stdio.h>
#include <stdlib.h>    
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <CL/cl.h>

float* init_matrix(float *A, cl_int row,cl_int col,int value);
void print_matrix(float *A, cl_int row, cl_int col);
int  check_result(float *res, float *c_res, float *a, float *b, cl_int m,cl_int n, cl_int p);
float* init_matrix(float *A, cl_int row,cl_int col,int value){
    int i = 0;
    for(i=0;i<row*col;i++){
            if(value ==0)
                A[i]=0;
            else 
                A[i] = value;
    }
    return A;
} 
void print_matrix(float *A, cl_int row, cl_int col){
    int i = 0;
    for(i=0;i<row*col;i++){
        if(i%col ==0)
            printf("\n");
        printf("%f ",A[i]);
    }
    printf("\n-------------------\n");
}
int  check_result(float *res, float *c_res, float *a, float *b, cl_int m,cl_int n, cl_int p){
    int i,k,flag = 0;
    for(i = 0; i < m*p; i++ ){
        int div_i = i/p;
        int mod_i = i%p;
        cl_float tmp = 0;
        for(k = 0; k < n;k++){
            tmp += a[ div_i * n + k ] * b[ p * k + mod_i ];
        }
        c_res[i] = tmp;
        if(c_res[i]!= res[i])
            flag = 1;
    }
    return flag;
}
cl_program load_program(cl_context context, cl_device_id device, const char* filename)
{
    FILE *fp = fopen(filename, "rt");
    size_t length;
    char *data;
    char *build_log;
    size_t ret_val_size;
    cl_program program = 0;
    cl_int status = 0;
    if(!fp) return 0;
   fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data = (char *)malloc(length + 1);
    fread(data, sizeof(char), length, fp);
    data[length] = '\0';
    program = clCreateProgramWithSource(context, 1, (const char **)&data, 0, 0);
    if (program == 0) return 0;
    status = clBuildProgram(program, 0, 0, 0, 0, 0);
    return program;
}

int main(int argc, char **argv)
{
    cl_int err = 0;
    cl_uint num = 0;
    cl_platform_id *platforms = NULL;
    cl_context_properties prop[3] = {0};
    cl_context context = 0;
    cl_device_id *devices = NULL;
    cl_command_queue queue = 0;
    cl_program program = 0;
    cl_mem cl_a = 0, cl_b = 0, cl_res = 0;
    cl_kernel adder = 0;
    cl_event event;
    int num_total_devices = 0;
    char devname[16][256] = {{0}};
    size_t cb, work_size;
    int i;
        
    cl_int m = 5, n = 4, p = 3;
    cl_float a[m*n],b[n*p],res[m*p];
    int flag=0;
    cl_float c_res[m*p];
    init_matrix(a,m,n,1);
    init_matrix(b,n,p,2);
    init_matrix(res,m,p,0);
    init_matrix(c_res,m,p,0);
    print_matrix(a,m,n);
    print_matrix(b,n,p);
    print_matrix(res,m,p);

    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num);
    printf("Found %d platforms:\n", num);
    for (i = 0; i < num; i++) {
        char str[1024];
        clGetPlatformInfo (platforms[i], CL_PLATFORM_NAME, 1024, str, NULL);
        printf("\t%d: %s\n", i, str);
    }
    prop[0] = CL_CONTEXT_PLATFORM;
    prop[1] = (cl_context_properties)platforms[0];
    prop[2] = 0;
    context = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, NULL);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    devices = (cl_device_id *)malloc(cb);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, 0);
    num_total_devices = cb / sizeof(cl_device_id);
    for (i = 0; i < num_total_devices; i++) {
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 256, devname[i], 0);
        printf("\t%d: %s", i, devname[i]);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &cb, 0);
        printf("  - %d\n", (int)cb);
    }
    queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, 0);
    program = load_program(context, devices[0], "matmulkernal.cl");
    cl_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * m * n, a, NULL);
    cl_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * n * p, b, NULL);
    cl_res = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_float) * m * p , res, NULL);
    adder = clCreateKernel(program, "test", &err);
    clSetKernelArg(adder, 0, sizeof(cl_mem), &cl_a);
    clSetKernelArg(adder, 1, sizeof(cl_mem), &cl_b);
    clSetKernelArg(adder, 2, sizeof(cl_mem), &cl_res);
    clSetKernelArg(adder, 3, sizeof(cl_int), &m);
    clSetKernelArg(adder, 4, sizeof(cl_int), &n);
    clSetKernelArg(adder, 5, sizeof(cl_int), &p);
    work_size = m * p;
    clWaitForEvents(1, &event);
    clFinish(queue);
    err = 0;
    flag = check_result(res,c_res,a,b,m,n,p);
    if(flag ==1){
        printf("matrix mulituple has some errors!! \n");
    }
    else{
        printf("matrix mulituple is correct!! \n");
    }
    print_matrix(a,m,n);
    print_matrix(b,n,p);
    print_matrix(res,m,p);
    print_matrix(c_res,m,p);
    clReleaseKernel(adder);
    clReleaseProgram(program);
    clReleaseMemObject(cl_a);
    clReleaseMemObject(cl_b);
    clReleaseMemObject(cl_res);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0; 
}

