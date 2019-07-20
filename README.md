# Intel_OCL_Snippets
***
## Autorun Kernels
* note: non_blocking channels are useful if packet lost in tolerated.
It seems the solution to avoid pipeline stalls between kernels is to use either multiple kernel queues or using Autorun kernels in conjuction with channels otherwise the kernels should be investigated by [Dynamic profiler](https://www.intel.com/content/www/us/en/programmable/documentation/mwh1391807516407.html#vcg1470763338276) and analysing stall and occupancy percentage on different stages]
