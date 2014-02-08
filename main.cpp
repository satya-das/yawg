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

#include "cppprog.h"
#include "cppwriter.h"

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <strstream>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

//////////////////////////////////////////////////////////////////////////

/**
 * Makes sure \a path uses preferred slash and it ends with slash
 */
inline void make_preferred_dir_format(boost::filesystem::path& path)
{
   path.make_preferred();
   if(path.native().back() != path.preferred_separator)
      path = path.native() + path.preferred_separator;
}

int main(int argc, char** argv)
{
	// Declare the supported options.
	bpo::options_description desc("Automatic wrapper generation");
	desc.add_options()
		("help,h",					"produce help message")
		("input-folder,i",			bpo::value<std::string>(), "Input folder from where header files are picked.")
		("output-folder,o",			bpo::value<std::string>(), "Output folder for emitting wrapper files.")
		("module,m",				bpo::value<std::string>(), "Module name.")
		;

	bpo::variables_map vm;
	bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
	bpo::notify(vm);

	if(vm.count("help"))
	{
		std::cerr << desc << "\n";
		return 1;
	}
	if(vm.count("input-folder") == 0)
	{
		std::cerr << "Error: Input folder not specified.\n\n" << desc << "\n";
		return 1;
	}
	if(vm.count("output-folder") == 0)
	{
		std::cerr << "Error: Output folder not specified.\n\n" << desc << "\n";
		return 1;
	}
	if(vm.count("module") == 0)
	{
		std::cerr << "Error: Module name not specified.\n\n" << desc << "\n";
		return 1;
	}

	bfs::path inputPath;
	bfs::path outputPath;

	if(vm.count("input-folder"))
		inputPath = vm["input-folder"].as<std::string>();
	if(vm.count("output-folder"))
		outputPath = vm["output-folder"].as<std::string>();

	if	(!bfs::is_directory(inputPath) ||
		(bfs::exists(outputPath) && !bfs::is_directory(outputPath))
		)
		return -1;
	make_preferred_dir_format(inputPath);
	make_preferred_dir_format(outputPath);

	bfs::create_directories(outputPath);

	YPyXlator pyXlator(inputPath, outputPath, vm["module"].as<std::string>());
	const CppProgram& pyWrapProg = pyXlator.getBindingProg();
	const CppCompoundArray& fileDOMs = pyWrapProg.getFileDOMs();
	CppWriter cppWriter;
	for(CppCompoundArray::const_iterator itr = fileDOMs.begin(); itr != fileDOMs.end(); ++itr)
	{
		const CppCompound* pyCompound = *itr;
		std::ofstream stm(pyCompound->name_);
		cppWriter.emit(pyCompound, stm);
	}
}
