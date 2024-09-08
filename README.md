# threads_performance
A single process (matrix multiplication on large matrices) is completed using multiple threads and time is noted to demonstrate how multithreading can affect performance.

* BASIC OPERATION : 
  Matrix multiplication, program gets matrices dimensions and min value and max value of each element from user and assigns random values to each element.

  For example, there are two matrices of size 1000x1000, and if we perform matrix multiplication, then elements are being multiplied 10^9 times and being added 10^8 times to generate resutant matrix

* IDEA : 
  Matrix multiplication process is divided among different threads row wise, example if there are n rows and 2 threads to be used, then each thread is assigned n/2 rows.
