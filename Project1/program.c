#include <stdio.h>
#include <stdlib.h>

int globalNumber = 5;      // global variable

void fillArray(int *arr, int size)
{
    for (int i = 0; i < size; i++)     // loop
    {
        arr[i] = i + 1;
    }
}

int sumArray(int *arr, int size)
{
    int sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }

    return sum;
}

void printResult(int sum)
{
    if (sum > 10)          // decision
    {
        printf("Sum is %d\n", sum);
    }
    else
    {
        printf("Small sum\n");
    }
}

int main()
{
    int size = globalNumber;

    int *numbers = malloc(size * sizeof(int));

    if (numbers == NULL)
    {
        return 1;
    }

    fillArray(numbers, size);

    int total = sumArray(numbers, size);

    printResult(total);

    free(numbers);

    return 0;
}
