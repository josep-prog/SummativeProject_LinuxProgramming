# 

| Name | Link |
| :---- | :---- |
| **DemoVideo** | **[Project 3 \[5pts\]: Developing a Python C Extension for High-Performance Data Processing](https://youtu.be/F7JdMwu5rWk)**  |
|  |  |

# 

# **Project 3: Developing a Python C Extension for High-Performance Data Processing**

## **Explanation, Design, Implementation, and Execution Guide**

## **Introduction**

Modern applications such as smart agriculture monitoring systems collect large amounts of data from sensors. These sensors may continuously measure environmental conditions such as soil moisture, temperature, humidity, and other important factors. When the amount of data becomes large, processing everything directly in Python can become slower because Python is an interpreted language.

To improve performance, this project develops a native C extension module that can be imported and used directly from Python. The purpose of the extension is to move computationally intensive numerical operations from Python into C, where calculations can be performed faster.

The extension module is called:

sensor\_analysis

The module receives sensor readings from Python as lists or tuples, processes the values inside C using the Python C API, and returns results back to Python as normal Python objects.

The implemented functions are:

* average(data) – calculates the arithmetic mean of sensor readings.  
* range\_value(data) – calculates the difference between maximum and minimum values.  
* variance(data) – calculates the sample variance.  
* count\_above(data, limit) – counts values greater than a given limit.  
* statistics(data) – returns a summary dictionary containing important statistics.

# **1\. Project Structure**

The project contains three main files:

sensor\_project/

│── sensor\_analysis.c  
│── setup.py  
│── test.py

Each file has a specific purpose:

### **sensor\_analysis.c**

This is the main C extension source file. It contains:

* C implementations of statistical functions.  
* Python C API integration.  
* Python method definitions.  
* Module initialization code.

### **setup.py**

This file contains the build configuration required to compile the C code into a Python extension module.

### **test.py**

This Python script tests the extension by:

* Importing the module.  
* Creating sample sensor data.  
* Calling every function.  
* Testing invalid input handling.

# 

# **2\. Using the Python C API**

The extension uses the Python C API to allow communication between Python and C.

Normally, Python works with Python objects:

Example:

data \= \[20.5, 25.7, 30.1\]

However, C works with normal C data types:

double value;

The Python C API provides functions that allow conversion between the two environments.

For example:

PySequence\_GetItem()

is used to access values from Python lists and tuples.

The value is then converted into a C double:

PyFloat\_AsDouble()

This allows all mathematical operations to happen directly in C.

The general process is:

Python List  
      |  
      v  
Python C API  
      |  
      v  
C double values  
      |  
      v  
Mathematical calculation  
      |  
      v  
Python Result

# **3\. Average Function**

## **Purpose**

The average() function calculates the arithmetic mean of all sensor readings.

The mathematical formula is:

Average=Sum of valuesNumber of valuesAverage \= \\frac{Sum\\ of\\ values}{Number\\ of\\ values}Average=Number of valuesSum of values​

Example:

Input:

\[20.0, 30.0, 40.0\]

Calculation:

(20 \+ 30 \+ 40\) / 3

Result:

30.0

The function processes every value once.

Time complexity:

O(n)

where n is the number of sensor readings.

# **4\. Range Function**

## **Purpose**

The range\_value() function finds the difference between the highest and lowest sensor readings.

The formula is:

Range=Maximum−MinimumRange \= Maximum \- MinimumRange=Maximum−Minimum

Example:

Input:

\[15.5, 25.0, 40.5\]

Maximum:

40.5

Minimum:

15.5

Result:

25.0

The function checks every element to find the minimum and maximum values.

Time complexity:

O(n)

# **5\. Variance Function**

## **Purpose**

The variance() function calculates the sample variance of the sensor readings.

Variance measures how spread out the values are from the average.

The formula used is:

Variance=∑(x−xˉ)2n−1Variance \= \\frac{\\sum(x-\\bar{x})^2}{n-1}Variance=n−1∑(x−xˉ)2​

Where:

* x represents each sensor value.  
* x̄ represents the average value.  
* n represents the number of samples.

Example:

For sensor values:

\[10, 20, 30\]

The function:

1. Calculates the average.  
2. Calculates the difference between each value and the average.  
3. Squares the differences.  
4. Divides by n-1.

The function requires at least two values because sample variance cannot be calculated from one sample.

Time complexity:

O(n)

# **6\. Count Above Function**

## **Purpose**

The count\_above() function counts the number of sensor readings greater than a given limit.

Example:

Input:

data \= \[20, 30, 40, 50\]

limit \= 35

Values above 35:

40, 50

Result:

2

The function checks each value and increases the counter when the condition is true.

Time complexity:

O(n)

# **7\. Statistics Function**

## **Purpose**

The statistics() function returns a Python dictionary containing a summary of the dataset.

The returned dictionary format is:

{  
    "samples": number\_of\_samples,  
    "average": average\_value,  
    "minimum": minimum\_value,  
    "maximum": maximum\_value  
}

Example:

Input:

\[20.5, 30.0, 40.5\]

Output:

{  
"samples": 3,  
"average": 30.33,  
"minimum": 20.5,  
"maximum": 40.5  
}

The function calculates all values in one pass through the data.

Time complexity:

O(n)

# **8\. Input Validation and Error Handling**

The extension checks that the input data is valid before processing.

The functions only accept:

* Python lists  
* Python tuples

Invalid example:

sensor\_analysis.average("hello")

The extension raises:

TypeError:  
Input must be a list or tuple

## **Empty Dataset Handling**

An empty dataset cannot produce meaningful statistical results.

Example:

sensor\_analysis.average(\[\])

The extension raises:

ValueError:  
Dataset cannot be empty

## **Variance Validation**

Sample variance requires at least two samples.

Example:

sensor\_analysis.variance(\[10\])

The extension raises:

ValueError:  
Need at least two samples

# **9\. Memory Management**

The extension avoids unnecessary memory allocation.

Instead of copying the Python list into a new C array, the program accesses values directly from the Python object.

This approach has several advantages:

* Uses less memory.  
* Avoids unnecessary copying.  
* Improves performance.

When accessing Python objects, the extension manages reference counting.

Example:

Py\_DECREF(item);

This releases temporary objects after they are no longer needed and prevents memory leaks.

# **10\. Numerical Accuracy Considerations**

Sensor data normally contains decimal values, such as:

23.5  
67.8  
45.2

Therefore, the extension uses the C data type:

double

instead of integers.

The double type provides enough precision for most sensor calculations and reduces rounding errors during mathematical operations.

# **11\. Building the Extension**

Before building, install the required tools.

## **Install Python development headers:**

sudo apt install python3-dev

Install setuptools:

pip install setuptools

## **Build the Extension**

Navigate to the project directory:

cd sensor\_project

Run:

python3 setup.py build

This compiles the C source file and creates the Python extension module.

The generated file will have a name similar to:

sensor\_analysis.cpython-312-x86\_64-linux-gnu.so

The .so file is the compiled Python extension.

# **12\. Running the Test Program**

Run:

python3 test.py

Example output:

Average: 28.5

Range: 12.3

Variance: 22.6

Above 30: 2

Statistics:

{  
 'samples': 5,  
 'average': 28.5,  
 'minimum': 22.8,  
 'maximum': 35.1  
}

Error: Dataset cannot be empty

# **13\. Execution Flow**

The complete execution process is:

Python Test Program

        |  
        v

Import sensor\_analysis

        |  
        v

Python C Extension Module

        |  
        v

Receive Python List/Tuple

        |  
        v

Convert Values to C double

        |  
        v

Perform Calculation in C

        |  
        v

Create Python Object Result

        |  
        v

Return Result to Python

# **Conclusion**

This project demonstrates how Python can be extended with C to improve performance in data processing applications. The extension module moves numerical calculations from Python into native C code while maintaining the simplicity of calling functions from Python.

The implementation uses the Python C API to safely exchange data between Python and C, validates input types, handles errors, manages memory correctly, and performs all calculations using efficient C operations.

This approach is especially useful in real-time applications such as smart agriculture systems, where thousands or millions of sensor readings must be processed quickly and efficiently. By combining Python's flexibility with C's performance, developers can build faster and more scalable data-processing systems.

