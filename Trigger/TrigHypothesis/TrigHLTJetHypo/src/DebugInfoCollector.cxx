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
  ss << "DEBUGInfoCollector: "
     << m_name << " no of msgs: " << m_info.size() << "\n\n";

  std::size_t msg_i {0};
  for(const auto& i: m_info){
    ss << "msg " << msg_i << ": " << i.first << '\n';
    ++msg_i;
    for(const auto& m : i.second){ss << m <<  '\n';}
  }
  return ss.str();
}


void DebugInfoCollector::write() const{
  auto report = this->toString();
  std::ofstream out(m_name+".log");
  out << report;   
}

