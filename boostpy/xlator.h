/*
The MIT License (MIT)

Copyright (c) 2014

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __YAWG_BOOST_PYTHON_XLATOR_H__
#define __YAWG_BOOST_PYTHON_XLATOR_H__

#include "utils.h"

#include "cppprog.h"
#include "cppdom.h"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

namespace bfs = boost::filesystem;

typedef std::vector<std::string>	stringlist;
typedef std::vector<bfs::path>		pathlist;

//////////////////////////////////////////////////////////////////////////

/**
 * 
 */
class BoostPythonXlator
{
public:
   BoostPythonXlator(const bfs::path& cppSourcePath, const bfs::path& pySourcePath, const std::string& moduleName);

private:

private:
   CppProgram	mCppProg;
   std::string	mModName;
   bfs::path	mBaseSrcDir;
   bfs::path	mBaseDstDir;
};

#endif //__YAWG_BOOST_PYTHON_XLATOR_H__
