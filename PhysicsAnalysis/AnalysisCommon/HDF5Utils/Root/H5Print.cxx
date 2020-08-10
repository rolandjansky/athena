/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HDF5Utils/H5Print.h"
#include <iomanip>

namespace H5Utils { namespace Print {
  std::ostream& operator<<(std::ostream& os, const H5::DataSet& ds)
  {
    os << os.fill() << ds.getObjName();
    return os;
  }
 
  std::ostream& operator<<(std::ostream& os, const H5::Group& group)
  {
    std::size_t indent = os.width();
    os << os.fill() << group.getObjName() << " {" << std::endl;
    for (std::size_t ii = 0; ii < group.getNumObjs(); ++ii) {
      H5G_obj_t childType = group.getObjTypeByIdx(ii);
      std::string childName = group.getObjnameByIdx(ii);
      switch(childType) {
        case H5G_GROUP:
          os << std::setw(indent+2) << group.openGroup(childName) << std::endl;
          break;
        case H5G_DATASET:
          os << std::setw(indent+2) << group.openDataSet(childName) << std::endl;
          break;
        default:
          // For now do nothing with other types - maybe in the future rethink
          // this?
          break;
      }
    }
    os << std::setw(indent) << os.fill() << "}";
    return os;
  }
} } //> end namespace H5Utils::Print
