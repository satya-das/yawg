#include "hello.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(hello) {
	boost::python::def("greet", greet;
}
