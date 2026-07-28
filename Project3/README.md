# Project 3: `sensor_analysis` — Python C Extension for High-Performance Data Processing

## Overview

`sensor_analysis` is a native Python C extension module that performs common
statistical operations over a collection of floating-point sensor readings
(soil moisture, temperature, humidity, etc.) directly in C, so a Python
caller processing thousands of readings avoids the overhead of a pure-Python
loop.

## Files

| File | Purpose |
|---|---|
| `sensor_analysis.c` | C extension source (Python C API implementation) |
| `setup.py` | Build configuration (setuptools) |
| `test.py` | Test/demo script exercising every function |

## Compilation

Build the extension in-place with:

```bash
python3 setup.py build_ext --inplace
```

This compiles `sensor_analysis.c` against the Python headers and produces a
shared object (e.g. `sensor_analysis.cpython-3xx-x86_64-linux-gnu.so`) in the
current directory, importable directly by Python.

## Execution

```bash
python3 test.py
```

### Expected output

Using the sample dataset in `test.py`, `data = [25.5, 30.2, 28.9, 35.1, 22.8]`:

```
Average: 28.5
Range: 12.3
Variance: 22.025000000000002
Above 30: 2
Statistics:
{'samples': 5, 'average': 28.5, 'minimum': 22.8, 'maximum': 35.1}
Error: Dataset cannot be empty
```

(`Variance` prints with trailing floating-point noise — 22.025 is not exactly
representable in IEEE 754 double precision — this is expected, not a bug.)

The final line comes from the script's boundary-condition test, which calls
`sensor_analysis.average([])` inside a `try/except` and prints the
`ValueError` raised by the module.

## Required Functions

### `average(data)`
Formula: `sum(x) / n`. Time complexity: O(n).

### `range_value(data)`
Formula: `max(x) - min(x)`. Time complexity: O(n) (single pass tracking a
running min/max).

### `variance(data)`
Sample variance: `sum((x - mean)^2) / (n - 1)`. Time complexity: O(n), but
makes **two passes** over the data (one to compute the mean, one to
accumulate squared deviations from it), since the mean is required before
any deviation can be calculated. Requires at least 2 samples (raises
`ValueError` otherwise, since sample variance is undefined for n < 2).

### `count_above(data, limit)`
Counts values strictly greater than `limit`. Time complexity: O(n).

### `statistics(data)`
Returns a dict: `{"samples": n, "average": ..., "minimum": ..., "maximum": ...}`.
Time complexity: O(n).

## Input Validation

`get_data()` rejects any argument that isn't a `list` or `tuple`
(`TypeError`) and rejects an empty sequence (`ValueError`) before any
numeric work begins.

Beyond container-level validation, every individual element is converted
through a shared helper, `as_double()`:

```c
static int as_double(PyObject *item, double *out)
{
    double value = PyFloat_AsDouble(item);
    if (value == -1.0 && PyErr_Occurred())
        return 0;
    *out = value;
    return 1;
}
```

`PyFloat_AsDouble` returns `-1.0` both for a legitimately negative-one value
*and* on conversion failure (e.g. the element is a string), so the only way
to distinguish the two is to check `PyErr_Occurred()` immediately after the
call. Every loop that pulls an element out of the input sequence checks this
return value and, on failure, releases the reference it holds and returns
`NULL` — propagating the `TypeError` Python's own conversion machinery
already raised, rather than silently treating a bad element as `-1.0`.

## Memory Management

- **Python → C conversion**: each element is fetched with
  `PySequence_GetItem` (which returns a new reference) and immediately
  converted to a C `double` via `as_double()`. The reference is released
  with `Py_DECREF` right after use, on both the success and failure paths.
- **No unnecessary dynamic allocation**: every function computes its result
  from C `double` locals in a single pass (or two passes for `variance`,
  see above) without ever building an intermediate C array — the input data
  already lives in the Python list/tuple, so copying it into a second
  buffer would be pure overhead.
- **C → Python conversion**: results are returned via `PyFloat_FromDouble`
  or `PyLong_FromLong`, which each create a new reference that is handed
  back to the interpreter (ownership transferred to the caller).
- **`statistics()` dict construction**: `PyDict_SetItemString` does **not**
  steal the reference to the value being inserted — it increments the
  refcount itself. So each temporary (`PyLong_FromLong`, `PyFloat_FromDouble`)
  is stored in a local, inserted into the dict, and then `Py_DECREF`'d
  immediately, avoiding a reference leak on every call.

## Numerical Accuracy Considerations

- All arithmetic is done in C `double` (IEEE 754 double precision), matching
  Python's native `float`, so no precision is lost converting between the
  two.
- `variance()` uses the two-pass mean-then-deviations method rather than the
  single-pass "sum of squares minus square of sum" shortcut, because the
  latter is numerically unstable (subject to catastrophic cancellation) for
  data with a large mean relative to its spread.
- `range_value()` and `statistics()` track running min/max with a single
  comparison per element rather than sorting, avoiding both unnecessary
  O(n log n) cost and any reordering of the input.
