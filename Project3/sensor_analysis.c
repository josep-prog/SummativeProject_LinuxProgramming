#include <Python.h>
#include <math.h>


/*
Convert Python list/tuple values into C double values.
The calculations are performed directly in C.
*/


/*
Convert a Python item to a C double, checking for conversion errors.
PyFloat_AsDouble returns -1.0 both for a legitimate -1.0 value and on
failure, so PyErr_Occurred() must be checked to tell them apart.
*/
static int as_double(PyObject *item, double *out)
{
    double value = PyFloat_AsDouble(item);

    if (value == -1.0 && PyErr_Occurred())
        return 0;

    *out = value;
    return 1;
}


int get_data(PyObject *data, Py_ssize_t *size)
{
    if (!PyList_Check(data) && !PyTuple_Check(data))
    {
        PyErr_SetString(PyExc_TypeError,
                        "Input must be a list or tuple");
        return 0;
    }

    *size = PySequence_Size(data);

    if (*size == 0)
    {
        PyErr_SetString(PyExc_ValueError,
                        "Dataset cannot be empty");
        return 0;
    }

    return 1;
}


/*
average(data)

Formula:

average = sum of values / number of values

Time complexity: O(n)
*/

static PyObject* average(PyObject *self, PyObject *args)
{
    PyObject *data;

    if (!PyArg_ParseTuple(args, "O", &data))
        return NULL;


    Py_ssize_t size;

    if (!get_data(data, &size))
        return NULL;


    double sum = 0;


    for(Py_ssize_t i = 0; i < size; i++)
    {
        PyObject *item = PySequence_GetItem(data, i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }

        sum += value;

        Py_DECREF(item);
    }


    return PyFloat_FromDouble(sum / size);
}



/*
range_value(data)

Formula:

maximum - minimum

Time complexity: O(n)
*/

static PyObject* range_value(PyObject *self, PyObject *args)
{
    PyObject *data;


    if (!PyArg_ParseTuple(args, "O", &data))
        return NULL;


    Py_ssize_t size;


    if (!get_data(data, &size))
        return NULL;


    double min;
    double max;


    PyObject *first = PySequence_GetItem(data,0);

    if (!as_double(first, &min))
    {
        Py_DECREF(first);
        return NULL;
    }

    max = min;

    Py_DECREF(first);



    for(Py_ssize_t i=1;i<size;i++)
    {
        PyObject *item = PySequence_GetItem(data,i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }


        if(value < min)
            min=value;


        if(value > max)
            max=value;


        Py_DECREF(item);
    }


    return PyFloat_FromDouble(max-min);
}



/*
variance(data)

Sample variance formula:

sum((x - mean)^2) / (n - 1)

Time complexity: O(n)
*/

static PyObject* variance(PyObject *self, PyObject *args)
{

    PyObject *data;


    if(!PyArg_ParseTuple(args,"O",&data))
        return NULL;


    Py_ssize_t size;


    if(!get_data(data,&size))
        return NULL;


    if(size < 2)
    {
        PyErr_SetString(PyExc_ValueError,
                        "Need at least two samples");
        return NULL;
    }


    double sum=0;


    for(Py_ssize_t i=0;i<size;i++)
    {
        PyObject *item=PySequence_GetItem(data,i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }

        sum += value;

        Py_DECREF(item);
    }


    double mean=sum/size;


    double result=0;


    for(Py_ssize_t i=0;i<size;i++)
    {
        PyObject *item=PySequence_GetItem(data,i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }

        result += pow(value-mean,2);

        Py_DECREF(item);
    }


    return PyFloat_FromDouble(result/(size-1));

}



/*
count_above(data,limit)

Counts values greater than limit.

Time complexity: O(n)
*/

static PyObject* count_above(PyObject *self, PyObject *args)
{

    PyObject *data;

    double limit;


    if(!PyArg_ParseTuple(args,"Od",&data,&limit))
        return NULL;


    Py_ssize_t size;


    if(!get_data(data,&size))
        return NULL;



    int count=0;


    for(Py_ssize_t i=0;i<size;i++)
    {
        PyObject *item=PySequence_GetItem(data,i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }


        if(value > limit)
            count++;


        Py_DECREF(item);
    }


    return PyLong_FromLong(count);

}



/*
statistics(data)

Returns dictionary:

{
samples,
average,
minimum,
maximum
}

Time complexity: O(n)
*/

static PyObject* statistics(PyObject *self, PyObject *args)
{

    PyObject *data;


    if(!PyArg_ParseTuple(args,"O",&data))
        return NULL;


    Py_ssize_t size;


    if(!get_data(data,&size))
        return NULL;



    double sum=0;

    double min;

    double max;



    PyObject *first=PySequence_GetItem(data,0);

    if (!as_double(first, &min))
    {
        Py_DECREF(first);
        return NULL;
    }

    max = min;

    Py_DECREF(first);



    for(Py_ssize_t i=0;i<size;i++)
    {

        PyObject *item=PySequence_GetItem(data,i);

        double value;

        if (!as_double(item, &value))
        {
            Py_DECREF(item);
            return NULL;
        }


        sum+=value;


        if(value<min)
            min=value;


        if(value>max)
            max=value;


        Py_DECREF(item);

    }


    PyObject *dict=PyDict_New();


    PyObject *samples_obj=PyLong_FromLong(size);
    PyDict_SetItemString(dict,"samples",samples_obj);
    Py_DECREF(samples_obj);


    PyObject *average_obj=PyFloat_FromDouble(sum/size);
    PyDict_SetItemString(dict,"average",average_obj);
    Py_DECREF(average_obj);


    PyObject *minimum_obj=PyFloat_FromDouble(min);
    PyDict_SetItemString(dict,"minimum",minimum_obj);
    Py_DECREF(minimum_obj);


    PyObject *maximum_obj=PyFloat_FromDouble(max);
    PyDict_SetItemString(dict,"maximum",maximum_obj);
    Py_DECREF(maximum_obj);


    return dict;

}



/* Method table */

static PyMethodDef methods[] =
{

    {"average", average, METH_VARARGS, "Calculate average"},

    {"range_value", range_value, METH_VARARGS,
     "Calculate range"},

    {"variance", variance, METH_VARARGS,
     "Calculate variance"},

    {"count_above", count_above, METH_VARARGS,
     "Count values above limit"},

    {"statistics", statistics, METH_VARARGS,
     "Return statistics dictionary"},


    {NULL,NULL,0,NULL}

};



/* Module definition */

static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "sensor_analysis",
    "Sensor analysis module",
    -1,
    methods
};



/* Module initialization */

PyMODINIT_FUNC PyInit_sensor_analysis(void)
{
    return PyModule_Create(&module);
}
