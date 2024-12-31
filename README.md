# RISC-V-Simulator
Project Description
Overview
In computer assignment 1, we will design a RISC-V single-cycle processor using a list of instructions provided later. Your task is first to design your datapath and controller based on what you have learned in class and then implement it in C++. 

To help you get started, you can download the skeleton code from the website. Along with this description and the skeleton code, we have uploaded several test files (assembly and binary) that you could use to test your code. 

Your processor should read a given trace (i.e., a text file that has the instructions), execute instructions one by one, and report the final value in a0 and a1 registers. Additionally, please also submit your design as a separate PDF document. Your grade will be determined by your performance on the Gradescope tests; however, submitting the PDF with your design is also required.

Instructions:
You need to implement the following list of instructions:

ADD, LUI, ORI, XOR, SRAI
LB, LW, SB, SW
BEQ, JAL


Code:
The entry point of your project is “cpusim.cpp”. The program should run like this: 
“./cpusim <inputfile.txt>”, 
and print the value of a0 and a1 in the terminal:
					“(a0,a1)”   {no space}
(for example, if a0=10, and a1= -8, then you should print (10,-8). You should use exactly this format, otherwise, our automated tests cannot evaluate your code.)

It is your choice how you want to structure your code (e.g., whether you want to have separate objects for each class, or you want to instantiate an object within another class, or even not use any class at all and utilize functions and structs, etc.). Our main suggestion is to use a separate function/class/struct for each unit. 
