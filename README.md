# Intel_OCL_Snippets
***
## Autorun Kernels
* note: non_blocking channels are useful if packet lost in tolorated.
It seems the solution to avoid stalls is to use either multiple kernel queues or using Autorun kernels in conjuction with channels
