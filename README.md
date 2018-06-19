# pipelined-processor-simulator
simulator should be capable of loading a specified MIPS binary file and generate the cycle-by-cycle simulation of the MIPS binary code. It should also produce/print the contents of registers, queues, and memory data for each cycle.

# Build
g++ MIPSsim.cpp -o MIPSsim

# Run
./MIPSsim sample.txt

# Files Note
MIPSsim.cpp: Source code
sample.txt: text file (MIPS) which be input as argument

# Others
