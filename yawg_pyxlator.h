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

#ifndef __YAWG_PYXLATOR_H__
#define __YAWG_PYXLATOR_H__

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
 * \brief Responsible to translate a C/C++ program into CPython binding.
 */
class YPyXlator
{
public:
	YPyXlator(const bfs::path& cppSourcePath, const bfs::path& pySourcePath, const std::string& moduleName);

	const CppProgram& getBindingProg() const	{ return mPyBindProg; }

private:

	CppCompound*	createModuleFile() const;
	CppVar*			createModuleMethods() const;
	CppFunction*	createModuleInit() const;
	CppVar*			createModuleDefn() const;
	CppCompound*	translateCppToPy(const CppCompound* cppFileDOM) const;

	std::string moduleInitFuncName() const						{ return "PyInit_" + mModName;		}
	std::string moduleMethodsVarName() const					{ return mModName + "Methods";		}
	std::string moduleFilename() const							{ return "ypw_" + mModName + ".cpp";}
	std::string moduleNameString() const						{ return strLit(mModName);			}
	std::string	modDefName() const								{ return "ypw_" + mModName + "Def";	}

private:
	static std::string wrapperName(const std::string& cppName)	{ return "ypw_" + cppName;			}

	static CppExprList* createMethodEntry(const CppFunction* cppFunc);
	static CppExprList* createMethodEntrySentinel();

private:

	CppProgram	mPyBindProg;
	CppProgram	mCppProg;
	std::string	mModName;
	bfs::path	mBaseSrcDir;
	bfs::path	mBaseDstDir;
	pathlist	mWrapHdrs;
};

#endif //__YAWG_PYXLATOR_H__
