/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HDF5Utils/CompressedTypes.h"

namespace {
  H5::DataType halfPrecisionFloat(int ebias = 15) {
    // start with native float
    H5::FloatType type(H5Tcopy(H5::PredType::NATIVE_FLOAT.getId()));

    // These definitions are copied from h5py, see:
    //
    //  https://github.com/h5py/h5py/blob/596748d52c351258c851bb56c8df1c25d3673110/h5py/h5t.pyx#L212-L217
    //
    type.setFields(15, 10, 5, 0, 10);
    type.setSize(2);
    type.setEbias(ebias);
    return type;
  }
}

namespace H5Utils {

  namespace internal {

    template <>
    H5::DataType getCompressedType<float>(Compression comp) {
      switch (comp) {
      case Compression::STANDARD: return H5Traits<float>::type;
      case Compression::HALF_PRECISION: return halfPrecisionFloat();
      case Compression::HALF_PRECISION_LARGE: return halfPrecisionFloat(5);
      default: throw std::logic_error("unknown float compression");
      }
    }

  }

}
