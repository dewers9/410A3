#! /usr/bin/env python

# switch to python3 if this is not running ^^^

import matplotlib.pyplot as plt
import numpy as np
import sys

file_types = ["nreg", "ndir", "nblk", "nchr", "nfifo", "nslink", "nsock"]

data = input("")

# split string into numbers
numbers_str = data.split()

# convert each number from string to integer
numbers = [int(num) for num in numbers_str]
print(numbers)

# plot the histogram of the count of files
plt.bar(file_types, numbers)
plt.xlabel('File Types')
plt.ylabel('Frequency')
plt.title('Frequency of File Typesssssss')
plt.show()
plt.savefig("my-histogram.jpg")