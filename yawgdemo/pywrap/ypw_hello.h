#include "yawg_python.h"
#include "Python.h"
#include "hello.h"
PyObject* ypw_greet(PyObject* self, PyObject* args)
{
	YPyObject yPyObj = greet();
	return yPyObj;
}
