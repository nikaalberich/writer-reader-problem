# writer-reader-problem
Write a program that creates three programs - A, B and C. 
Program B implements the reader process. Program C implements the writer process. 
Program A runs 8 instances of program B and 5 instances of program C.
The name of the file for exchanging information between the reader and writer processes is passed to the processes by program A through an environment variable.  
Program A passes the names of the semaphores for synchronization to the processes through command line parameters. 
Readers and writers display their work on the screen.
