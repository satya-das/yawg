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

#include "yawg_pyxlator.h"
#include "utils.h"

#include "cppprog.h"

//////////////////////////////////////////////////////////////////////////

static const std::string pyHeader		= "\"Python.h\"";
static const std::string ypyHeader		= "\"yawg_python.h\"";
static const std::string pyObjName		= "PyObject";
static const std::string yPyObjName		= "YPyObject";
static const std::string pyModCreate	= "PyModule_Create";
static const std::string pyModDefHead	= "PyModuleDef_HEAD_INIT";
static const std::string pyModDef		= "PyModuleDef";

//////////////////////////////////////////////////////////////////////////

YPyXlator::YPyXlator(const bfs::path& cppSourcePath, const bfs::path& pySourcePath, const std::string& moduleName)
	: mCppProg(cppSourcePath)
	, mModName(moduleName)
	, mBaseSrcDir(cppSourcePath)
	, mBaseDstDir(pySourcePath)
{
	const CppCompoundArray& fileDOMs = mCppProg.getFileDOMs();
	// Iterate through all files and translate them for binding.
	for(CppCompoundArray::const_iterator itr = fileDOMs.begin(); itr != fileDOMs.end(); ++itr)
	{
		CppCompound* inCompound	= *itr;
		CppCompound* pyCompound = translateCppToPy(inCompound);
		bfs::path pyPath = pySourcePath / relative_path(cppSourcePath, inCompound->name_).string();
		std::string filename = "ypw_" + pyPath.filename().string();
		pyPath.remove_filename();
		pyPath /= filename;
		mWrapHdrs.push_back(pyPath);
		pyCompound->name_ = pyPath.string();
		mPyBindProg.addCppDom(pyCompound);
	}
	bfs::path pyModuleSrc = pySourcePath / moduleFilename();
	CppCompound* moduleFile = createModuleFile();
	moduleFile->name_ = pyModuleSrc.string();
	mPyBindProg.addCppDom(moduleFile);
}

CppCompound* YPyXlator::translateCppToPy(const CppCompound* cppFileDOM) const
{
	CppCompound* yawgPyDOM	= new CppCompound(kCppFile);
	yawgPyDOM->addMember(new CppInclude(ypyHeader));
	yawgPyDOM->addMember(new CppInclude(pyHeader));
	std::string srcHdr = relative_path(mBaseSrcDir, cppFileDOM->name_).string();
	yawgPyDOM->addMember(new CppInclude(strLit(srcHdr)));
	for(CppObjArray::const_iterator objItr = cppFileDOM->members_.begin(); objItr != cppFileDOM->members_.end(); ++objItr)
	{
		CppObj* cppObj = *objItr;
		if(cppObj->objType_ == CppObj::kFunction)
		{
			CppFunction* cFunc	= (CppFunction*) cppObj;
			CppParam self		= makeCppParam(new CppVar(kUnknownProt, pyObjName, 0, kFuncParam, 1, kNoRef, "self"));
			CppParam args		= makeCppParam(new CppVar(kUnknownProt, pyObjName, 0, kFuncParam, 1, kNoRef, "args"));
			CppVarType* retType	= new CppVarType(kUnknownProt, pyObjName, 0, 1, kNoRef);
			CppFunction* pyFunc	= new CppFunction(kUnknownProt, wrapperName(cFunc->name_), retType, self, args);
			CppCompound* fnDefn	= new CppCompound(kBlock);
			CppExpr* call = new CppExpr(cFunc->name_, kFunctionCall, CppExprAtom());
			if(cFunc->retType_->isVoid())
			{
				fnDefn->addMember(call);
				fnDefn->addMember(new CppExpr("Py_RETURN_NONE", kNone));
			}
			else
			{
				CppVar* yPyObj = new CppVar(kUnknownProt, yPyObjName, 0, 0, 0, kNoRef, "yPyObj");
				yPyObj->assign_ = call;
				fnDefn->addMember(yPyObj);
				CppExpr* ret = new CppExpr("yPyObj", kNone);
				ret->flags_ |= CppExpr::kReturn;
				fnDefn->addMember(ret);
			}
			pyFunc->defn_ = fnDefn;
			yawgPyDOM->members_.push_back(pyFunc);
		}
	}

	return yawgPyDOM;
}

CppCompound* YPyXlator::createModuleFile() const
{
	CppCompound* moduleFile = new CppCompound(kCppFile);
	moduleFile->addMember(new CppInclude(pyHeader));
	const CppCompoundArray& fileDOMs = mCppProg.getFileDOMs();
	// Iterate through all files and translate them for binding.
	for(pathlist::const_iterator hdrItr = mWrapHdrs.begin(); hdrItr != mWrapHdrs.end(); ++hdrItr)
	{
		std::string wrapperHdr = relative_path(mBaseDstDir, *hdrItr).string();
		moduleFile->addMember(new CppInclude(strLit(wrapperHdr)));
	}
	CppFunction* moduleInit = createModuleInit();
	moduleFile->addMember(moduleInit);

	return moduleFile;
}

CppVar* YPyXlator::createModuleMethods() const
{
	CppExprList* methodInitList = new CppExprList;
	const CppCompoundArray& fileDOMs = mCppProg.getFileDOMs();
	// Iterate through all files and translate them for binding.
	for(CppCompoundArray::const_iterator itr = fileDOMs.begin(); itr != fileDOMs.end(); ++itr)
	{
		const CppCompound* cppFileDOM = *itr;
		for(CppObjArray::const_iterator objItr = cppFileDOM->members_.begin(); objItr != cppFileDOM->members_.end(); ++objItr)
		{
			CppObj* cppObj = *objItr;
			if(cppObj->objType_ == CppObj::kFunction)
				methodInitList->push_back(new CppExpr(createMethodEntry((CppFunction*) cppObj), CppExpr::kInitializer));
		}
	}
	methodInitList->push_back(new CppExpr(createMethodEntrySentinel(), CppExpr::kInitializer));
	CppVar* methodDef = new CppVar(kUnknownProt, "PyMethodDef", kStatic, kArray, 0, kNoRef, moduleMethodsVarName());
	methodDef->assign_ = new CppExpr(methodInitList, CppExpr::kInitializer);
	return methodDef;
}

CppExprList* YPyXlator::createMethodEntry(const CppFunction* cppFunc)
{
	CppExprList* exprList = new CppExprList;
	exprList->push_back(new CppExpr('"' + cppFunc->name_ + '"', kNone));
	exprList->push_back(new CppExpr(wrapperName(cppFunc->name_), kNone));
	exprList->push_back(new CppExpr("METH_VARARGS", kNone));
	return exprList;
}

CppExprList* YPyXlator::createMethodEntrySentinel()
{
	CppExprList* exprList = new CppExprList;
	exprList->push_back(new CppExpr("NULL", kNone));
	exprList->push_back(new CppExpr("NULL", kNone));
	return exprList;
}

CppVar* YPyXlator::createModuleDefn() const
{
	CppExprList* defnInit = new CppExprList;
	defnInit->push_back(new CppExpr(pyModDefHead, kNone));
	defnInit->push_back(new CppExpr(moduleNameString(), kNone));
	defnInit->push_back(new CppExpr("NULL", kNone));
	defnInit->push_back(new CppExpr("-1", kNone));
	defnInit->push_back(new CppExpr(moduleMethodsVarName(), kNone));

	CppVar* modDefn = new CppVar(kUnknownProt, pyModDef, kStatic, 0, 0, kNoRef, modDefName());
	modDefn->assign_ = new CppExpr(defnInit, CppExpr::kInitializer);
	return modDefn;
}

CppFunction* YPyXlator::createModuleInit() const
{
	CppVarType* retType = new CppVarType(kUnknownProt, "PyMODINIT_FUNC", 0, 0, kNoRef);
	CppFunction* moduleInitFunc = new CppFunction(kUnknownProt, moduleInitFuncName(), retType, NULL, 0);
	CppCompound* initDef = new CppCompound(kBlock);
	initDef->addMember(createModuleMethods());
	initDef->addMember(createModuleDefn());
   CppExpr* modCreate = new CppExpr(pyModCreate, kFunctionCall, new CppExpr(modDefName(), kRefer));
   modCreate->flags_ |= CppExpr::kReturn;
   initDef->addMember(modCreate);
   moduleInitFunc->defn_ = initDef;
	return moduleInitFunc;
}
