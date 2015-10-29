/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/Error.h"

namespace L1Topo {
  std::ostream& operator<<(std::ostream& os, const Error e){
    std::string text = "Unknown Error";
    auto it = errorText.find(e);
    if (it != errorText.end()){
      text = it->second;
    }  
    os << text;
    return os;
  }
  
  std::ostream& operator<<(std::ostream& os, const std::vector<Error>& v){
    os << "[";
    std::string prefix="";
    for (auto e : v){
      os << prefix << e;
      prefix=", ";
    }
    os << "]";
    return os;
  }

} // namespace L1Topo
