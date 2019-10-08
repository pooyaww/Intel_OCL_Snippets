// Each iteration needs 11 clock cycles for floating points
// so data dependencies exists between iterations
__kernel void double_add_1 (__global double *arr, int N, __global double *result) {
    double temp_sum = 0;
    for (int i = 0; i < N; ++i) {
        temp_sum += arr[i];
    }
    *result = temp_sum;
}
