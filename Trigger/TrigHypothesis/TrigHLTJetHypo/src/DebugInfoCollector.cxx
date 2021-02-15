/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./DebugInfoCollector.h"
#include <sstream>
#include <fstream>
#include <cmath>

DebugInfoCollector::DebugInfoCollector(const std::string& name,
				       bool byTime):
  m_name(name), m_byTime(byTime){
  constexpr bool nanoseconds{false};  // false: units are us
  m_timer = JetTrigTimer(nanoseconds);
  m_timer.start();
}

DebugInfoCollector::~DebugInfoCollector(){write();}

void DebugInfoCollector::collect(const std::string& key,
                                 const std::string& info){
  m_info[key].emplace_back(m_timer.elapsed(), info);
}

std::string DebugInfoCollector::toStringByMsgKey() const {
  /* print out messages in msg key order */

  std::stringstream ss;
  ss << "DEBUGInfoCollector: "
     << m_name << " no of msgs: " << m_info.size() << "\n\n";

  std::size_t msg_i {0};
  for(const auto& i: m_info){
    ss << "msg " << msg_i << ": " << i.first << '\n';
    ++msg_i;
    for(const auto& m : i.second){
      // msg time followe by msg
      ss << int(m.first) << " " << m.second <<  '\n';}
  }
  return ss.str();
}

std::string DebugInfoCollector::toStringByTime() const {
  /* print out messages in time order */

  std::map<unsigned long, std::string> by_time;
  
  for(const auto& p : m_info){
    for(const auto& tm : p.second){
      // tm.first = time from vector of (time, msg)
      // p.first = msg key
      // tm.second = msg at time tm.first

      auto timestamp = static_cast<unsigned long>(tm.first);
      by_time[timestamp] = p.first + " " + tm.second;
    }
  }
	     
  std::stringstream ss;
  ss << "DEBUGInfoCollector: "
     << m_name << " no of msgs: " << m_info.size() << "\n\n";

  for(const auto& i: by_time){
    ss << i.first << " " << i.second << '\n';
  }
  return ss.str();
}

std::string DebugInfoCollector::toString() const {
  return m_byTime ? toStringByTime() : toStringByMsgKey();
}


void DebugInfoCollector::write() const{
  auto report = this->toString();
  std::ofstream out(m_name+".log");
  out << report;   
}

