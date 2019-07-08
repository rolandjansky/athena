/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./DebugInfoCollector.h"
#include <sstream>
#include <fstream>

DebugInfoCollector::DebugInfoCollector(const std::string& name):m_name(name){}

void DebugInfoCollector::collect(const std::string& key,
                                 const std::string& info){
  m_info[key].push_back(info);
}

std::string DebugInfoCollector::toString() const {
  std::stringstream ss;
  ss << "DEBUGInfoCollector: " << m_name << " [" << m_info.size() << "]\n";
  for(const auto& i: m_info){
    ss << "  " << i.first << '\n';
    for(const auto& m : i.second){
      ss << "    "  << m <<  '\n';
    }
  }
  return ss.str();
}


void DebugInfoCollector::write() const{
  auto report = this->toString();
  std::ofstream out(m_name+".log");
  out << report;   
}

