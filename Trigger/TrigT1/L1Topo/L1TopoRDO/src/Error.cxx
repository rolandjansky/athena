/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/Error.h"

namespace L1Topo {

} // namespace L1Topo

std::ostream& operator<<(std::ostream& os, const L1Topo::Error e){
  std::string text = "Unknown Error";
  auto it = L1Topo::errorText.find(e);
  if (it != L1Topo::errorText.end()){
    text = it->second;
  }  
  os << text;
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<L1Topo::Error>& v){
  os << "[";
  std::string prefix="";
  for (auto e : v){
    os << prefix << e;
    prefix=", ";
  }
  os << "]";
  return os;
}
