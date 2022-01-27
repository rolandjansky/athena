/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitoredRange.h"
#include "MonitorBase.h"
#include "CounterBase.h"

#include "AthenaKernel/getMessageSvc.h"

MonitorBase::MonitorBase(const std::string& name, const MonitoredRange* parent) : 
  m_msgStream(nullptr, name.c_str()),
  m_name(name), 
  m_parent(parent) {
    m_msgStream.setMsgSvc(Athena::getMessageSvc());
}


const std::string& MonitorBase::getName() const {
  return m_name;
}


const MonitoredRange* MonitorBase::getParent() const {
  return m_parent;
}


StatusCode MonitorBase::endEvent(float weight) {
  for (auto& nameCounterPair : m_counters ) {
    ATH_CHECK( nameCounterPair.second->endEvent(weight) );
  }
  return StatusCode::SUCCESS;
}


TH1* MonitorBase::bookGetPointer(TH1* hist, const std::string& tDir) const {
  std::string dir = getName();
  if (tDir != "") {
    dir += "/";
    dir += tDir;
  }
  return getParent()->bookGetPointer(hist, dir);
}


bool MonitorBase::counterExists(const std::string& name) const {
  return (m_counters.count(name) == 1);
}


CounterBase* MonitorBase::getCounter(const std::string& name) {
  auto it = m_counters.find(name);
  if (it != m_counters.end()) {
    return it->second.get();
  }
  // If no counter exists, then we make a new one on the fly & return it.
  auto result = m_counters.insert( std::make_pair(name, newCounter(name)) ); // newCounter is specialised
  it = result.first;
  return it->second.get();
}


MsgStream& MonitorBase::msg() {
  return m_msgStream;
}

MsgStream& MonitorBase::msg(const MSG::Level lvl) {
  return m_msgStream << lvl;
}

bool MonitorBase::msgLvl(const MSG::Level lvl){
  return lvl >= m_msgStream.level();
}