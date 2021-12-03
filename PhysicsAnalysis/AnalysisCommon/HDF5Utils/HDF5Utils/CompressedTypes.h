// this is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMPRESSED_TYPES_H
#define COMPRESSED_TYPES_H

#include "CompressionEnums.h"
#include "H5Traits.h"

#include "H5Cpp.h"

#include <stdexcept>

namespace H5Utils {

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
