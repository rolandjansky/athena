/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoConfigGlobal.h"

#include <iostream>

using namespace std;



TXC::L1TopoConfigGlobal::~L1TopoConfigGlobal() {}

void TXC::L1TopoConfigGlobal::addTopoConfigElement(const std::string & name, const std::string & value) {
   m_topoConfigElements.push_back(TopoConfigElement(name,value));
}

void TXC::L1TopoConfigGlobal::addTopoConfigElement(const TopoConfigElement & config) {
   m_topoConfigElements.push_back(config);
}

namespace TXC {
    
std::ostream & operator<<(std::ostream &o, const L1TopoConfigGlobal & topoconfig) {
   o << "General configuration (<TopoConfig>):" << endl;
   for(const TXC::TopoConfigElement& ce : topoconfig.getTopoConfigs())
      o << "        " << ce.name << ": " << ce.value << endl;

  return o;
}

} // namespace TXC
