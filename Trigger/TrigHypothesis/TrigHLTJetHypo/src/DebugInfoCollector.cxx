/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./DebugInfoCollector.h"
#include <sstream>
#include <fstream>

DebugInfoCollector::DebugInfoCollector(const std::string& name):m_name(name){}

void DebugInfoCollector::collect(const std::string& key,
                                 const std::string& info){
  m_info[key] += info;
  m_calls[key] += 1;
}

std::string DebugInfoCollector::toString() const {
  std::stringstream ss;
  ss << "DEBUGInfoCollector: " << m_name << '\n';
  for(const auto& i: m_info){
    ss << i.first << '\n';
    ss << "   "  << i.second << " calls " << m_calls.at(i.first) << '\n';
  }
  return ss.str();
}


void DebugInfoCollector::write() const{
  auto report = this->toString();
  std::ofstream out(m_name+".log");
  out << report;   
}

