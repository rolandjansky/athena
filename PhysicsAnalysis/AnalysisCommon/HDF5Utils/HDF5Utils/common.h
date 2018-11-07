/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef H5UTILS_COMMON_H
#define H5UTILS_COMMON_H

namespace H5 {
  class CompType;
  class DataSpace;
  class DSetCreatPropList;
}

#include "H5Traits.h"
#include "H5Cpp.h"

#include <vector>

namespace H5Utils {
  namespace internal {

    // packing utility, to create a compact on-disk representation of
    // the HDF5 type.
    H5::CompType packed(H5::CompType in);

    // functions which are used by writers to set up the data spaces
    // and datasets
    H5::DataSpace getUnlimitedSpace(const std::vector<hsize_t>& max_length);
    H5::DSetCreatPropList getChunckedDatasetParams(
      const std::vector<hsize_t>& max_length,
      hsize_t batch_size);
    std::vector<hsize_t> getStriding(std::vector<hsize_t> max_length);

    // writer error handling
    void throwIfExists(const std::string& name, const H5::Group& in_group);
    void printDestructorError(const std::string& msg);

  }
}

#endif
