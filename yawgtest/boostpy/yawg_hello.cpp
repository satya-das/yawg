#include "hello.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(boostpy) {
	boost::python::def("greet", greet);
}
