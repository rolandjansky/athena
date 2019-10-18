/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HDF5Utils/IH5Merger.h"

namespace H5Utils {

  IH5Merger::~IH5Merger() {}

  void IH5Merger::merge(H5::H5File& target, const H5::H5File& source)
  {
    merge(
        static_cast<H5::Group&>(target),
        static_cast<const H5::Group&>(source) );
  }

  H5::Group IH5Merger::createFrom(
      H5::H5Location& targetLocation,
      const H5::Group& source)
  {
    H5::Group newGroup = targetLocation.createGroup(source.getObjName() );
    merge(newGroup, source);
    return newGroup;
  }
} //> end namespace H5Utils
