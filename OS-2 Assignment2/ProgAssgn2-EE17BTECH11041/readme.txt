Run the program files with c++11 or above. i.e by using $ g++ -std=c++11 file.cpp OR $ g++-6 file.cpp
Mention the refrence to pthread while compiling
$ g++ -std=c++11 file.cpp -pthread -o out_file
Then the required log files will be generated on runnimg the output file.
**Do not open the log files untill the execution of the program otherwise the threads will lose synchronization.**
