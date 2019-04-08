/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./DebugInfoCollector.h"
#include <sstream>
#include <fstream>
void DebugInfoCollector::collect(const std::string& key,
                                 const std::string& info){
  m_info[key] += info;
  m_calls[key] += 1;
}

std::string DebugInfoCollector::toString() const noexcept{
  std::stringstream ss;
  ss << "DEBUGInfoCollector:\n";
  for(const auto& i: m_info){
    ss << i.first << '\n';
    ss << "   "  << i.second << " calls " << m_calls.at(i.first) << '\n';
  }
  return ss.str();
}


void DebugInfoCollector::write(std::string name,
                                      uint32_t run,
                                      uint32_t event) const {
  auto report = this->toString();
  std::stringstream ss;
  ss << "jetHypo_" << name << "_" << run << "_" << event;
  std::ofstream out(ss.str());
  out << report;   
}


void DebugInfoCollector::write(std::string name, std::size_t id) const {
  auto report = this->toString();
  std::stringstream ss;
  ss << "jetHypo_" << name << "_" << id;
  std::ofstream out(ss.str());
  out << report;
  out.close();
}
