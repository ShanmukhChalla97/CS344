#!/usr/bin/env python3

#Author: Shanmukh Challa
#Class: CS 344 - Operating Systems
#Student ID: 9324869998
#References used:
#   https://www.guru99.com/reading-and-writing-files-in-python.html
#   https://stackoverflow.com/questions/2030053/random-strings-in-python
#   https://stackoverflow.com/questions/3996904/generate-random-integers-between-0-and-9

import string
import random
import sys

#Create three files and add write permissions to each
createFile1 = open("file1.txt", "w+")
createFile2 = open("file2.txt", "w+")
createFile3 = open("file3.txt", "w+")

#Generate random lowercase strings and assign them in randomString* variables
randomString1 = ''.join(random.choice(string.ascii_lowercase) for i in range(10)) + "\n"
randomString2 = ''.join(random.choice(string.ascii_lowercase) for i in range(10)) + "\n"
randomString3 = ''.join(random.choice(string.ascii_lowercase) for i in range(10)) + "\n"

#Write each of the randomStrings in the appropriate files
createFile1.write(randomString1)
createFile2.write(randomString2)
createFile3.write(randomString3)

#Close the files
createFile1.close()
createFile2.close()
createFile3.close()

#Open the files again, but with read permission only
openFile1 = open("file1.txt", "r+")
openFile2 = open("file2.txt", "r+")
openFile3 = open("file3.txt", "r+")

#Print out the contents in each of the files
print(openFile1.read())
print(openFile2.read())
print(openFile3.read())

#Close the files
openFile1.close()
openFile2.close()
openFile3.close()

#Generate two random numbers in the range of 1 and 42
num1 = random.randint(1, 42)
num2 = random.randint(1, 42)

#Print out the two random numbers, and the product of the two random numbers
print(num1)
print(num2)
print(num1 * num2)
