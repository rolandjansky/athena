/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "H5Cpp.h"
#include <HDF5Utils/DefaultMerger.h>

// Suppress a warning from boost.
// (Binaries apparently include boost with -I rather than -isystem.)
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc11-extensions"
#endif
#include <boost/program_options.hpp>
#ifdef __clang__
# pragma clang diagnostic pop
#endif

#include <boost/algorithm/string/split.hpp> 
#include <boost/algorithm/string/trim.hpp> 
#include <iostream>
#include <iomanip>

/**
 * A simple script to merge HDF5 files.
 *
 * This script is intended to read in a list of HDF5 files and create a new file
 * with all datasets contained inside them concatenated along a particular axis.
 */


int main(int argc, char* argv[]) {
  // The options
  std::string outputFile = "merged.h5";
  std::string inCSV = "";
  std::vector<std::string> inputFiles;
  hsize_t mergeAxis = 0;
  int chunkSize = -1;
  bool requireSameFormat = true;
  std::size_t bufferSizeMB = 100;
  std::size_t bufferSizeRows = -1;
  bool overwrite = false;
  bool inPlace = false;

  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("output,o", po::value(&outputFile), "The output file.")
    ("input,i", po::value(&inCSV), "A comma separated list of input files")
    ("allowDifferentFormats", po::bool_switch(&requireSameFormat),
     "Allow input files to have different formats.")
    ("mergeAxis,a", po::value(&mergeAxis),
     "The axis along which to merge datasets")
    ("chunkSize,c", po::value(&chunkSize),
     "The chunk size to use along the merge axis. If left negative uses the same chunks as the first input.")
    ("bufferSizeMB,B", po::value(&bufferSizeMB),
     "The size of the buffer to use in MB. Cannot be set with 'bufferSizeRows'")
    ("bufferSizeRows,b", po::value(&bufferSizeRows),
     "The size of the buffer to use in rows. Cannot be set with 'bufferSizeMB'")
    ("overwrite,w", po::bool_switch(&overwrite),
     "Overwrite the output file if it already exists. Cannot be set with 'in-place'")
    ("in-place,p", po::bool_switch(&inPlace),
     "The output file is modified in place. Cannot be set with 'overwrite'")
    ("help,h", "Print this message and exit.");

  po::options_description hidden;
  hidden.add_options()
    ("inputFiles", po::value(&inputFiles), "The input files");
  po::positional_options_description positional;
  positional.add("inputFiles", -1); //> All positional arguments are input files

  po::variables_map vm;
  po::options_description allOptions;
  allOptions.add(desc).add(hidden);

  po::store(
      po::command_line_parser(argc, argv).
        options(allOptions).
        positional(positional).
        run(),
      vm);
  // Do help before notify - notify will verify input arguments which we don't
  // want to do with help
  if (vm.count("help") ) {
    std::cout << "Merge HDF5 files. Usage:" << std::endl << std::endl;
    std::cout << "hdf5-merge [options] [--input input1,input2,... | input1 [input2 ...]]" << std::endl << std::endl;
    std::cout << desc << std::endl;
    return 0;
  }
  po::notify(vm);

  if (inCSV.size() > 0) {
    std::vector<std::string> splitCSV;
    boost::algorithm::split(splitCSV, inCSV, boost::algorithm::is_any_of(",") );
    for (const std::string& i : splitCSV)
      inputFiles.push_back(boost::algorithm::trim_copy(i) );
  }
  if (inputFiles.size() == 0) {
    std::cerr << "You must specify at least 1 input file!" << std::endl;
    return 1;
  }
  if (overwrite && inPlace) {
    std::cerr << "You cannot specify both overwrite and in-place!" << std::endl;
    return 1;
  }
  if (vm.count("bufferSizeMB") && vm.count("bufferSizeRows") ) {
    std::cerr << "You cannot specify both bufferSizeMB and bufferSizeRows!" << std::endl;
    return 1;
  }
  std::size_t buffer;
  bool bufferInRows;
  if (vm.count("bufferSizeRows") ) {
    buffer = bufferSizeRows;
    bufferInRows = true;
  }
  else {
    // Default used if neither was set or if bufferSizeMB is set
    std::size_t MB = 1024*1024;
    if (std::size_t(-1) / bufferSizeMB < MB)
      throw std::overflow_error(
          "Requested buffer size would overflow the register!");
    buffer = bufferSizeMB * MB;
    bufferInRows = false;
  }

  // Make the merger
  H5Utils::DefaultMerger merger(
      mergeAxis, chunkSize, requireSameFormat, buffer, bufferInRows);

  // Make the output file
  H5::H5File fOut(outputFile,
      overwrite ? H5F_ACC_TRUNC : (inPlace ? H5F_ACC_RDWR : H5F_ACC_EXCL) );
  // Loop over the input files and merge them
  for (const std::string& inName : inputFiles) {
    std::cout << "Merging file " << inName << std::endl;
    H5::H5File fIn(inName, H5F_ACC_RDONLY);
    merger.merge(fOut, fIn);
  }
    

  return 0;
}
