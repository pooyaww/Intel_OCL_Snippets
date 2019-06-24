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




channel int chan_in;
channel int chan_out;



__attribute__((max_global_work_dim(0)))
__kernel void reader(global int *data_in, int size) {
 for (int i = 0; i < size; ++i) {
	write_channel_intel(chan_in, data_in[i]);
 }
}

__attribute__((max_global_work_dim(0)))
__attribute__((autorun))

__kernel void plusOne() {


 while(1)
 {
	 int input = read_channel_intel(chan_in);
	 write_channel_intel(chan_out, input+1);
 }

}

__attribute__((max_global_work_dim(0)))
__kernel void writer(global int *data_out, int size) {
	for (int i = 0; i < size; ++i) {
		data_out[i] = read_channel_intel(chan_out);
 }	
 

}

