#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>


static PyObject *
c_backend_info(PyObject *self, PyObject *args)
{
    return PyUnicode_FromString("rollingstats C backend is working");
}


static int
get_double_item(PyObject *seq, Py_ssize_t index, double *out)
{
    PyObject *item = PySequence_GetItem(seq, index);

    if (item == NULL) {
        return -1;
    }

    double value = PyFloat_AsDouble(item);
    Py_DECREF(item);

    if (PyErr_Occurred()) {
        return -1;
    }

    *out = value;
    return 0;
}


static int
validate_window_c(Py_ssize_t n, int window)
{
    if (n <= 0) {
        PyErr_SetString(PyExc_ValueError, "x must not be empty");
        return -1;
    }

    if (window <= 0) {
        PyErr_SetString(PyExc_ValueError, "window must be positive");
        return -1;
    }

    if ((Py_ssize_t)window > n) {
        PyErr_SetString(PyExc_ValueError, "window must not be larger than len(x)");
        return -1;
    }

    return 0;
}


static int
set_float_item(PyObject *result, Py_ssize_t index, double value)
{
    PyObject *py_value = PyFloat_FromDouble(value);

    if (py_value == NULL) {
        return -1;
    }

    if (PyList_SetItem(result, index, py_value) < 0) {
        Py_DECREF(py_value);
        return -1;
    }

    return 0;
}


static PyObject *
prepare_sequence_and_window(PyObject *args, PyObject **seq, int *window, Py_ssize_t *n)
{
    if (!PyArg_ParseTuple(args, "Oi", seq, window)) {
        return NULL;
    }

    *n = PySequence_Length(*seq);

    if (*n < 0) {
        return NULL;
    }

    if (validate_window_c(*n, *window) < 0) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *
c_rolling_sum(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    double current_sum = 0.0;

    for (Py_ssize_t i = 0; i < (Py_ssize_t)window; i++) {
        double value = 0.0;

        if (get_double_item(seq, i, &value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += value;
    }

    if (set_float_item(result, 0, current_sum) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    Py_ssize_t result_index = 1;

    for (Py_ssize_t i = (Py_ssize_t)window; i < n; i++) {
        double new_value = 0.0;
        double old_value = 0.0;

        if (get_double_item(seq, i, &new_value) < 0 ||
            get_double_item(seq, i - (Py_ssize_t)window, &old_value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += new_value - old_value;

        if (set_float_item(result, result_index, current_sum) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        result_index++;
    }

    return result;
}


static PyObject *
c_rolling_mean(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    double current_sum = 0.0;

    for (Py_ssize_t i = 0; i < (Py_ssize_t)window; i++) {
        double value = 0.0;

        if (get_double_item(seq, i, &value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += value;
    }

    if (set_float_item(result, 0, current_sum / (double)window) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    Py_ssize_t result_index = 1;

    for (Py_ssize_t i = (Py_ssize_t)window; i < n; i++) {
        double new_value = 0.0;
        double old_value = 0.0;

        if (get_double_item(seq, i, &new_value) < 0 ||
            get_double_item(seq, i - (Py_ssize_t)window, &old_value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += new_value - old_value;

        if (set_float_item(result, result_index, current_sum / (double)window) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        result_index++;
    }

    return result;
}


static PyObject *
c_rolling_min(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    for (Py_ssize_t start = 0; start < result_len; start++) {
        double current_min = 0.0;

        if (get_double_item(seq, start, &current_min) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        for (Py_ssize_t j = start + 1; j < start + (Py_ssize_t)window; j++) {
            double value = 0.0;

            if (get_double_item(seq, j, &value) < 0) {
                Py_DECREF(result);
                return NULL;
            }

            if (value < current_min) {
                current_min = value;
            }
        }

        if (set_float_item(result, start, current_min) < 0) {
            Py_DECREF(result);
            return NULL;
        }
    }

    return result;
}


static PyObject *
c_rolling_max(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    for (Py_ssize_t start = 0; start < result_len; start++) {
        double current_max = 0.0;

        if (get_double_item(seq, start, &current_max) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        for (Py_ssize_t j = start + 1; j < start + (Py_ssize_t)window; j++) {
            double value = 0.0;

            if (get_double_item(seq, j, &value) < 0) {
                Py_DECREF(result);
                return NULL;
            }

            if (value > current_max) {
                current_max = value;
            }
        }

        if (set_float_item(result, start, current_max) < 0) {
            Py_DECREF(result);
            return NULL;
        }
    }

    return result;
}


static int
initial_sum_and_sum_sq(PyObject *seq, int window, double *sum, double *sum_sq)
{
    *sum = 0.0;
    *sum_sq = 0.0;

    for (Py_ssize_t i = 0; i < (Py_ssize_t)window; i++) {
        double value = 0.0;

        if (get_double_item(seq, i, &value) < 0) {
            return -1;
        }

        *sum += value;
        *sum_sq += value * value;
    }

    return 0;
}


static double
variance_from_sums(double sum, double sum_sq, int window)
{
    double mean = sum / (double)window;
    double mean_sq = sum_sq / (double)window;
    double var = mean_sq - mean * mean;

    if (var < 0.0 && var > -1e-12) {
        var = 0.0;
    }

    return var;
}


static PyObject *
c_rolling_var(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    double current_sum = 0.0;
    double current_sum_sq = 0.0;

    if (initial_sum_and_sum_sq(seq, window, &current_sum, &current_sum_sq) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    double var = variance_from_sums(current_sum, current_sum_sq, window);

    if (set_float_item(result, 0, var) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    Py_ssize_t result_index = 1;

    for (Py_ssize_t i = (Py_ssize_t)window; i < n; i++) {
        double new_value = 0.0;
        double old_value = 0.0;

        if (get_double_item(seq, i, &new_value) < 0 ||
            get_double_item(seq, i - (Py_ssize_t)window, &old_value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += new_value - old_value;
        current_sum_sq += new_value * new_value - old_value * old_value;

        var = variance_from_sums(current_sum, current_sum_sq, window);

        if (set_float_item(result, result_index, var) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        result_index++;
    }

    return result;
}


static PyObject *
c_rolling_std(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    double current_sum = 0.0;
    double current_sum_sq = 0.0;

    if (initial_sum_and_sum_sq(seq, window, &current_sum, &current_sum_sq) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    double var = variance_from_sums(current_sum, current_sum_sq, window);

    if (set_float_item(result, 0, sqrt(var)) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    Py_ssize_t result_index = 1;

    for (Py_ssize_t i = (Py_ssize_t)window; i < n; i++) {
        double new_value = 0.0;
        double old_value = 0.0;

        if (get_double_item(seq, i, &new_value) < 0 ||
            get_double_item(seq, i - (Py_ssize_t)window, &old_value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        current_sum += new_value - old_value;
        current_sum_sq += new_value * new_value - old_value * old_value;

        var = variance_from_sums(current_sum, current_sum_sq, window);

        if (set_float_item(result, result_index, sqrt(var)) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        result_index++;
    }

    return result;
}


static PyObject *
c_rolling_zscore(PyObject *self, PyObject *args)
{
    PyObject *seq = NULL;
    int window = 0;
    Py_ssize_t n = 0;

    PyObject *prepared = prepare_sequence_and_window(args, &seq, &window, &n);

    if (prepared == NULL) {
        return NULL;
    }

    Py_DECREF(prepared);

    Py_ssize_t result_len = n - (Py_ssize_t)window + 1;
    PyObject *result = PyList_New(result_len);

    if (result == NULL) {
        return NULL;
    }

    double current_sum = 0.0;
    double current_sum_sq = 0.0;

    if (initial_sum_and_sum_sq(seq, window, &current_sum, &current_sum_sq) < 0) {
        Py_DECREF(result);
        return NULL;
    }

    for (Py_ssize_t start = 0; start < result_len; start++) {
        Py_ssize_t last_index = start + (Py_ssize_t)window - 1;
        double last_value = 0.0;

        if (get_double_item(seq, last_index, &last_value) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        double mean = current_sum / (double)window;
        double var = variance_from_sums(current_sum, current_sum_sq, window);
        double std = sqrt(var);
        double z = 0.0;

        if (std != 0.0) {
            z = (last_value - mean) / std;
        }

        if (set_float_item(result, start, z) < 0) {
            Py_DECREF(result);
            return NULL;
        }

        if (start + 1 < result_len) {
            double old_value = 0.0;
            double new_value = 0.0;

            if (get_double_item(seq, start, &old_value) < 0 ||
                get_double_item(seq, start + (Py_ssize_t)window, &new_value) < 0) {
                Py_DECREF(result);
                return NULL;
            }

            current_sum += new_value - old_value;
            current_sum_sq += new_value * new_value - old_value * old_value;
        }
    }

    return result;
}


static PyMethodDef crolling_methods[] = {
    {"backend_info", c_backend_info, METH_NOARGS, "Return information about the C backend."},
    {"rolling_sum", c_rolling_sum, METH_VARARGS, "Compute rolling sums."},
    {"rolling_mean", c_rolling_mean, METH_VARARGS, "Compute rolling means."},
    {"rolling_min", c_rolling_min, METH_VARARGS, "Compute rolling minima."},
    {"rolling_max", c_rolling_max, METH_VARARGS, "Compute rolling maxima."},
    {"rolling_var", c_rolling_var, METH_VARARGS, "Compute rolling population variances."},
    {"rolling_std", c_rolling_std, METH_VARARGS, "Compute rolling population standard deviations."},
    {"rolling_zscore", c_rolling_zscore, METH_VARARGS, "Compute rolling z-scores."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef crolling_module = {
    PyModuleDef_HEAD_INIT,
    "_crolling",
    "C backend for rollingstats.",
    -1,
    crolling_methods
};


PyMODINIT_FUNC
PyInit__crolling(void)
{
    return PyModule_Create(&crolling_module);
}
