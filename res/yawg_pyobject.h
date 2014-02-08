#ifndef __YAWG_PYTHON_OBJ_H__
#define __YAWG_PYTHON_OBJ_H__

#include "Python.h"

//////////////////////////////////////////////////////////////////////////

class YPyObject
{
public:
	YPyObject(PyObject* pyObj);
	YPyObject(const char* s);

	YPyObject(const YPyObject& yPyObj);

	~YPyObject();

public:
	operator PyObject* () const;

private:
	PyObject* mPyobj;
};

//////////////////////////////////////////////////////////////////////////

inline YPyObject::YPyObject(PyObject* pyObj)
	: mPyobj(pyObj)
{
	Py_XINCREF(mPyobj);
}

inline YPyObject::YPyObject(const char* s)
	: mPyobj(Py_BuildValue("s", s))
{
}

inline YPyObject::YPyObject(const YPyObject& yPyObj)
	: mPyobj(yPyObj.mPyobj)
{
	Py_XINCREF(mPyobj);
}

inline YPyObject::~YPyObject()
{
	Py_XDECREF(mPyobj);
}

inline YPyObject::operator PyObject* () const
{
	Py_INCREF(mPyobj);
	return mPyobj;
}

#endif //__YAWG_PYTHON_OBJ_H__
