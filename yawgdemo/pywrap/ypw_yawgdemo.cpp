#include "Python.h"
#include "ypw_hello.h"
PyMODINIT_FUNC PyInit_yawgdemo()
{
	static PyMethodDef yawgdemoMethods[] = {
		{"greet", ypw_greet, METH_VARARGS}, 
		{NULL, NULL}
	};
	static PyModuleDef ypw_yawgdemoDef = {PyModuleDef_HEAD_INIT, "yawgdemo", NULL, -1, yawgdemoMethods};
	return PyModule_Create(&ypw_yawgdemoDef);
}
