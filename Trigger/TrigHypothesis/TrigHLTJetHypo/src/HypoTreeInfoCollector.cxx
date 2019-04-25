/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./HypoTreeInfoCollector.h"
#include <sstream>
#include <fstream>

void HypoTreeInfoCollector::collect(const std::string& key,
                                 const std::string& info){
  m_info[key] += info;
}

std::string HypoTreeInfoCollector::toString() const {
  std::stringstream ss;
  ss << "HypoTreeInfoCollector:\n";
  for(const auto& i: m_info){
    ss << i.first << "\n   "  << i.second << '\n';
  }
  return ss.str();
}


void HypoTreeInfoCollector::write(std::string name) const {
  auto report = this->toString();
  std::stringstream ss;
  ss << "HypoTree_" << name;
  std::ofstream out(ss.str());
  out << report;
  out.close();
}
