// floating point op requires 11 cycles so II=12 we need a shift register of size  II_cycles+1 = 13
#define II_CYCLES 12
__kernel double_add_2 (__global double *arr, int N, __global double *result) {
    //create shift register with II_CYCLE+1 elements
    double shift_reg[II_CYCLES+1];
    //Initialize all elements of the register to 0
    for (int i = 0; i < II_CYCLES + 1; ++i)
        shift_reg[i] = 0;
    //Iterate through every element of input array
    for(int i = 0l i < N; ++i) {
        //if N > II_CYCLE, add to shift_reg[0] to preserve values
        shift_reg[II_CYCLES] = shift_reg[0] + arr[i];

        #pragma unroll
        //shift every element of shift register
        for(int j = 0; j < II_CYCLES; ++j)
            shift_reg[j] = shift_reg[j +1];
    }
    //sum every element of shift register
    double temp_sum = 0;
    for(int i = 0; i < II_CYCLES; ++i)
        temp_sum += shift_reg[i];

    *result = temp_sum;
}
