***** README *****

Open in terminal the folder with the files: census.c, censusADT.c, census.h and censusADT.h. (Verify the existence of them)

Compile & Linkedit with the following command:
gcc -g census.c censusADT.c -o census -Wall -pedantic -std=c99

After successfully compiling & linkediting, execute the program with:
./census < PATH

Where PATH is the path to the entry source file, the folder should also include a few example csv files for testing purposes.
