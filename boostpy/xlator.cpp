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

#include "xlator.h"
#include "utils.h"

#include "cppprog.h"
#include "cppwriter.h"

//////////////////////////////////////////////////////////////////////////

BoostPythonXlator::BoostPythonXlator(const bfs::path& cppSourcePath, const bfs::path& pySourcePath, const std::string& moduleName)
   : mCppProg(cppSourcePath)
   , mModName(moduleName)
   , mBaseSrcDir(cppSourcePath)
   , mBaseDstDir(pySourcePath)
{
   const CppCompoundArray& fileDOMs = mCppProg.getFileDOMs();
   // Iterate through all files and translate them for binding.
   for (auto fileCompund : fileDOMs)
   {
      bfs::path pyPath = pySourcePath / relative_path(cppSourcePath, fileCompund->name_).string();
      std::string filename = "yawg_" + pyPath.filename().string();
      pyPath.remove_filename();
      pyPath /= filename;
      pyPath.replace_extension(".cpp");

      std::ofstream pyStm(pyPath.native(), std::ios_base::out);

      pyStm << "#include " << bfs::path(fileCompund->name_).filename() << "\n";
      pyStm << "#include <boost/python.hpp>\n\n";
      pyStm << "BOOST_PYTHON_MODULE(" << moduleName << ") {\n";
      CppWriter::Indentation indent;
      ++indent;
      for(auto cppObj : fileCompund->members_)
      {
         switch(cppObj->objType_)
         {
            case CppObj::kFunction:
            {
               CppFunction* func = (CppFunction*) cppObj;
               pyStm << indent << "boost::python::def(\"" << func->name_ << "\", " << func->name_ << ");\n";
               break;
            }
         }
      }
      pyStm << --indent << "}\n";
   }
}
