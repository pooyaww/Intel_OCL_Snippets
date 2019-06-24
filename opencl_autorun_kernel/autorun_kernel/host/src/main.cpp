//Copyright (c) <2017> Intel Corporation

//Permission is hereby granted, free of charge, to any person obtaining a
//copy of this software and associated documentation files (the
//"Software"), to deal in the Software without restriction, including
//without limitation the rights to use, copy, modify, merge, publish,
//distribute, sublicense, and/or sell copies of the Software, and to
//permit persons to whom the Software is furnished to do so, subject to
//the following conditions:

//The above copyright notice and this permission notice shall be included
//in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include "CL/opencl.h"
#include <string.h>
#include "AOCLUtils/aocl_utils.h"

static const char* kernel_name = "autorun_kernel";
using namespace std;
using namespace aocl_utils;

static cl_platform_id platform = NULL;
static cl_device_id device = NULL;
static cl_context context = NULL;
static cl_command_queue fpga_to_host_queue = NULL;
static cl_command_queue host_to_fpga_queue = NULL;
static cl_kernel in_kernel = NULL;
static cl_kernel out_kernel = NULL;
static cl_kernel kernel = NULL;

static cl_program program = NULL;
static cl_int status = 0;

cl_mem input_buf;
cl_mem output_buf;

bool init() 
{
	if(!setCwdToExeDir()) {
		printf("init error\n");
		return false;
    }
		
	cl_uint num_platforms;
	// Get the OpenCL platform.
	status = clGetPlatformIDs(1, &platform, &num_platforms);
	if(platform == NULL) {
printf("ERROR: Unable to find Intel(R) FPGA OpenCL platform.\n");
		return false;
	}

	// Query the available OpenCL devices.
	cl_uint num_devices;
	  
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &num_devices);
	if(status != CL_SUCCESS) {
		
		printf("Failed clGetDeviceIDs.\n");
		return false;
	}

	// Create the context.
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
	if(status != CL_SUCCESS) printf("Failed to create context");
	
	// Create the command queue.
	host_to_fpga_queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
	if(status != CL_SUCCESS) printf("Failed to create command queue 1\n");
  
	fpga_to_host_queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status);
	if(status != CL_SUCCESS) printf("Failed to create command queue for crc\n");
  
	std::string binary_file = getBoardBinaryFile("autorun_kernel", device);
	static const char* autorun_kernel_name = "autorun_kernel.aocx";
	printf("\n Using AOCX: %s\n\n", autorun_kernel_name);
	program = createProgramFromBinary(context, autorun_kernel_name, &device, 1);
	if(status != CL_SUCCESS) {
		printf("Failed clCreateProgramWithBinary.\n");
		return false;
	}
  
	// Build the program that was just created.
	status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
	if(status != CL_SUCCESS) printf("Failed to build program\n");
  
	const char *kernel_name1 = "reader";  // Kernel name, as defined in the CL file
	in_kernel = clCreateKernel(program, kernel_name1, &status);
	if(status != CL_SUCCESS) printf("Failed to create kernel 1\n");
	const char *kernel_name2 = "writer";  
	out_kernel = clCreateKernel(program, kernel_name2, &status);
	if(status != CL_SUCCESS) printf("Failed to create kernel 2\n");
  
	return true;
}

// Free the resources allocated during initialization
void cleanup() 
{	
	//free kernel/queue/program/context
	if(kernel)
		clReleaseKernel(kernel);
	if(in_kernel)
		clReleaseKernel(in_kernel);
	if(out_kernel)
		clReleaseKernel(out_kernel);
	if(host_to_fpga_queue)
		clReleaseCommandQueue(host_to_fpga_queue);
	if(fpga_to_host_queue)
		clReleaseCommandQueue(fpga_to_host_queue);	
	if(program)
		clReleaseProgram(program);
	if(context)
		clReleaseContext(context);	
	
	//free in/out buffers
	if(input_buf)
		clReleaseMemObject(input_buf);
	if(output_buf)
		clReleaseMemObject(output_buf);
}

int main() 
{
	int insize = 10;
	int outsize = 10;
	unsigned int *input = (unsigned int *)malloc(insize*sizeof(int)); 
	unsigned int *output = (unsigned int *)malloc(outsize*sizeof(int)); 
	
	if (!input) printf("error in allocating input\n");
	if (!output) printf("error in allocating output\n");

	
	printf(" Input data: \n");
	for (int i =0; i < insize; ++i)
	{
		input[i] = i;
		printf("%d, ", input[i]);
	}
	printf("\n kernel function: Each Input_data will be added 1 \n");
	
	if (!init()) return false;

	input_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, insize * sizeof(unsigned int), NULL, &status);
    if(status != CL_SUCCESS) printf( "Failed to create input buffer\n");	

    output_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, outsize * sizeof(unsigned int), NULL, &status);
    if(status != CL_SUCCESS) printf( "Failed to create output_buf\n");
	
	cl_event write_event;
	cl_event finish_event;
    status = clEnqueueWriteBuffer(host_to_fpga_queue, input_buf, CL_FALSE, 0, insize * sizeof(unsigned int), input, 0, NULL, &write_event);
    if(status!=CL_SUCCESS) printf("Failed to transfer input\n");

	clWaitForEvents(0, &finish_event);
printf("\n\n write event status = %d \n ",status );
	
    unsigned argi = 0;
	cl_event in_kernel_event;
	cl_event out_kernel_event;
    
    status = clSetKernelArg(in_kernel, argi++, sizeof(cl_mem), &input_buf);
    if(status!=CL_SUCCESS) printf("Failed to set argument %d on kernel 1\n", argi - 1);
	status = clSetKernelArg(in_kernel, argi++, sizeof(cl_int), (void *) &insize);
	if(status!=CL_SUCCESS) printf("Failed to set argument %d on kernel 1\n", argi - 1);	
	printf("\n after arg set for in_kernel \n");
	argi = 0;
    status = clSetKernelArg(out_kernel, argi++, sizeof(cl_mem), &output_buf);
    if(status!=CL_SUCCESS) printf("Failed to set argument %d on kernel 1\n", argi - 1);	
	status = clSetKernelArg(out_kernel, argi++, sizeof(cl_int), (void *) &outsize);
	if(status!=CL_SUCCESS) printf("Failed to set argument %d on kernel 1\n", argi - 1);	
	printf("\n after arg set for out_kernel \n");
	status = clEnqueueTask(host_to_fpga_queue, in_kernel, 1, &write_event, &in_kernel_event);
	if(status!=CL_SUCCESS) printf("Failed to launch kernel\n");
 
	status = clEnqueueTask(fpga_to_host_queue, out_kernel, 0, NULL, &out_kernel_event);

	if(status!=CL_SUCCESS) printf("Failed to launch kernel\n");
	printf("\n after kernels queued! \n\n");
	status = clEnqueueReadBuffer(fpga_to_host_queue, output_buf, CL_FALSE, 0, outsize*sizeof(unsigned int), output, 1, &out_kernel_event, &finish_event);

	

printf("\n\n Read event status status = %d \n ",status );

	if (status != CL_SUCCESS) printf("read error\n");
	//printf("\n status %d: CL_SUCCESS \n", status);

	clWaitForEvents(1, &finish_event);

	printf("\n after wait \n");
	printf("Output data: \n");
	for (int i = 0; i< outsize; ++i)
		printf("%d, ", output[i]);
	
	// Self test to check kernel autorun is launch correctly 
	unsigned test_result = 0;
	for (int i = 0; i< outsize; ++i)
	{	
		if((input[i]+1) != output[i])
		test_result = 1;	 // as output incorrect, mark as wrong !
    }
	if (test_result == 0) 
		printf(" \n\n autorun kernel is launched successfully ! \n\n");
	else
		printf(" \n\n  autorun  kernel is launched failed ! \n\n");		

		
	clReleaseEvent(in_kernel_event);
	clReleaseEvent(out_kernel_event);
	clReleaseEvent(finish_event);
	cleanup();
	return 0;
}


