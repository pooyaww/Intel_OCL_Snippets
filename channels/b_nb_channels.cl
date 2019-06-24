//Blocking Channel Writes
channel long chan;

/*Defines the kernel which reads eight bytes (size of long) from global memory, and passes this data to the channel.*/ 
__kernel void kernel_write_channel( __global const long * src ) {
  for (int i = 0; i < N; i++) {
     //Writes the eight bytes to the channel.
     write_channel_intel(chan, src[i]);
  }
}


//Nonblocking Channel Writes
channel long worker0, worker1;
__kernel void producer( __global const long * src ) {
  for(int i = 0; i < N; i++)  {
        
    bool success = false;
    do {
      success = write_channel_nb_intel(worker0, src[i]);
      if(!success) {
        success = write_channel_nb_intel(worker1, src[i]);
      }
    }
    while(!success);
  }
}

//Blocking Channel Reads
channel long chan;

/*Defines the kernel, which reads eight bytes (size of long) from the channel and writes it back to global memory.*/
__kernel void kernel_read_channel (__global long * dst); {
  for (int i = 0; i < N; i++) {
     //Reads the eight bytes from the channel.
     dst[i] = read_channel_intel(chan);
  }
}

// Nonblocking Channel Reads
channel long chan;

__kernel void kernel_read_channel (__global long * dst) {
  int i = 0;
  while (i < N) {
    bool valid0, valid1;
    long data0 = read_channel_nb_intel(chan, &valid0);
    long data1 = read_channel_nb_intel(chan, &valid1);
    if (valid0) {
       process(data0);
    }
    if (valid1) {
       process(data1); 
    } 
  }
}