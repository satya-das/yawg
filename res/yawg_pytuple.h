#ifndef __YAWG_PYTHON_TUPLE_H__
#define __YAWG_PYTHON_TUPLE_H__

#include "yawg_pyobject.h"

#include "tupleobject.h"

//////////////////////////////////////////////////////////////////////////

class YPyTuple
{
public:
	YPyTuple(PyObject* pyTuple);

	YPyObject operator[](Py_ssize_t idx) const;

private:
	PyObject* mPyTuple;
};

inline YPyTuple::YPyTuple(PyObject* pyTuple)
	: mPyTuple(pyTuple)
{
}

inline YPyObject YPyTuple::operator [] (Py_ssize_t idx) const
{
	return PyTuple_GetItem(mPyTuple, idx);
}

#endif //__YAWG_PYTHON_TUPLE_H__
