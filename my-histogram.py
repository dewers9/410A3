#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import sys

# for line in sys.stdin:
#     line = line.strip()
#     print("You entered:", line)

# split into numbers after each space
# numbers_str = content.split()

# # Convert each number from string to integer
# numbers = [int(num) for num in numbers_str]
# numbers = numbers[1:]

file_types = ["nreg", "ndir", "nblk", "nchr", "nfifo", "nslink", "nsock"]

data = input("")

# Convert each number from string to integer
numbers_str = data.split()

# Convert each number from string to integer
numbers = [int(num) for num in numbers_str]
print(numbers)

plt.bar(file_types, numbers)
plt.xlabel('File Types')
plt.ylabel('Frequency')
plt.title('Frequency of File Types')

plt.show()
plt.savefig("my-histogram.jpg")