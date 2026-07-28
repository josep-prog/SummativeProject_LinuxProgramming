import sensor_analysis


data = [25.5, 30.2, 28.9, 35.1, 22.8]


print("Average:",
      sensor_analysis.average(data))


print("Range:",
      sensor_analysis.range_value(data))


print("Variance:",
      sensor_analysis.variance(data))


print("Above 30:",
      sensor_analysis.count_above(data,30))


print("Statistics:")

print(
    sensor_analysis.statistics(data)
)


# Test invalid input

try:

    sensor_analysis.average([])

except Exception as e:

    print("Error:", e)
