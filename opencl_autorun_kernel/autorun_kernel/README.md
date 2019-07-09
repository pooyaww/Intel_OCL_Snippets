## Autorun Kernels
Concurrent kernels and channels allow the organisation of system components as adata-flow process network (DPN). A comfort feature that saves the labour of having toexplicitly start every single component of such a DPN are autorun kernels. Kernels thatdo not need parameters by the host, e.g. an interconnect, an intermediate computationor reorder unit, are declared as autorun.
```
__attribute__((max_global_work_dim(0)))
__attribute__((autorun))
__kernelvoidinterconnect() {
#ifdefEMULATOR
printf("interconnect␣starting.\n");
#endif
```

Unlike host-started kernels, autorun kernels require no logic for interaction with anOpenCL command queue. This reduces the complexity of these components, allowsthe hardware synthesis to further optimise parts, and saves FPGA space.
