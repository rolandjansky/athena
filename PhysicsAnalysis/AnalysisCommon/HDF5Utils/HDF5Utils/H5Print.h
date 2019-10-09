/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HDF5Utils_H5Print_H
#define HDF5Utils_H5Print_H
#include <H5Cpp.h>
#include <iostream>

/**
 * @file H5Print.h
 * @author Jon Burr
 *
 * Helper functions to print out basic information about H5 groups.
 * To use, pull them into the namespace of your function with
 * using namespace H5Utils::Print;
 * std::cout << h5File << std::endl;
 */

namespace H5Utils { namespace Print {
  /// Print information about a dataset
  std::ostream& operator<<(std::ostream& os, const H5::DataSet& ds);
  /// Print information about a group
  std::ostream& operator<<(std::ostream& os, const H5::Group& group);
} } //> end namespace H5Utils::Print
#endif //> !HDF5Utils_H5Print_H
