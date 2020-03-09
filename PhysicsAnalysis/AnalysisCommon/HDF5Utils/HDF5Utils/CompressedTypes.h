// this is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMPRESSED_TYPES_H
#define COMPRESSED_TYPES_H

#include "H5Traits.h"

#include "H5Cpp.h"

#include <stdexcept>

namespace H5Utils {

  enum class Compression {STANDARD, HALF_PRECISION};

  namespace internal {
    template <typename T>
    H5::DataType getCompressedType(Compression comp) {
      if (comp != Compression::STANDARD) {
        throw std::logic_error("compression not supported for this type");
      }
      return H5Traits<T>::type;
    }
    template <>
    H5::DataType getCompressedType<float>(Compression comp);
  }
}

#endif
