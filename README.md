# Leak-Detector
To use it:

 - Add Memory_Leak_Detector.c to the directory containing your C file 

 - Include the Memory_Leak_Detector.c file in your C file 

		#include "Memory_Leak_Detector.c"
     

 - In your main function at the following line:

		 atexit( reportLeak );

Once you have executed the C file, you can find a summary of memory leaks in the "Memory_Leak_Info.txt" , created in the same directory.  
