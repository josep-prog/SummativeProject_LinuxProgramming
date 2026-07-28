from setuptools import setup, Extension


module = Extension(
    "sensor_analysis",
    sources=["sensor_analysis.c"]
)


setup(
    name="sensor_analysis",
    version="1.0",
    ext_modules=[module]
)
