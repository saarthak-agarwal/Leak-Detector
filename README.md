# Leak-Detector
To use:

 - Add Memory_Leak_Detector.c to the directory containing your C program 

 - Include the Memory_Leak_Detector.c file in your C program 

		#include "Memory_Leak_Detector.c"
     

 - In the main function add the following line:

		 atexit( reportLeak );

Once the C program has been executed, a summary of the memory leaks can be found in the "Memory_Leak_Info.txt" created in the same directory.  
