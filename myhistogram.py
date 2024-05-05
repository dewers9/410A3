import matplotlib.pyplot as plt
import numpy as np
import sys

with open('python_contents', 'r') as file:
    content = file.read()

# for line in sys.stdin:
#     line = line.strip()
#     print("You entered:", line)

# split into numbers after each space
numbers_str = content.split()

# Convert each number from string to integer
numbers = [int(num) for num in numbers_str]
numbers = numbers[1:]

file_types = ["nreg", "ndir", "nblk", "nchr", "nfifo", "nslink", "nsock"]

plt.bar(file_types, numbers)
plt.xlabel('File Types')
plt.ylabel('Frequency')
plt.title('Frequency of File Types')
plt.savefig("my-histogram.jpg")