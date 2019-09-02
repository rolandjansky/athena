/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCostAnalysis.h"
#include "MonitoredRange.h"
#include "MonitorBase.h"

MonitoredRange::MonitoredRange(const std::string& name, TrigCostAnalysis* parent) : 
  m_name(name),
  m_parent(parent),
  m_cachedLifetimeHistPtr(nullptr),
  m_monitors() 
{
  std::string hisSvcName = getName() + "_walltime";
  std::unique_ptr<TH1F> hist = std::make_unique<TH1F>(hisSvcName.c_str(), "Walltime;;Seconds", 1, -.5, .5);
  m_cachedLifetimeHistPtr = bookGetPointer(hist.release()); // Now owned by HistSvc
}


const std::string& MonitoredRange::getName() const {
  return m_name;
}


TrigCostAnalysis* MonitoredRange::getParent() const {
  return m_parent;
}


TH1* MonitoredRange::bookGetPointer(TH1* hist, const std::string& tDir) const {
  std::string dir = getName();
  if (tDir != "") {
    dir += "/";
    dir += tDir;
  }
  return getParent()->bookGetPointer_fwd(hist, dir); // Use forwarding method. Actual member is protected
}


std::set< std::unique_ptr<MonitorBase> >& MonitoredRange::getMonitors() {
  return m_monitors;
}


StatusCode MonitoredRange::addMonitor(std::unique_ptr<MonitorBase> monitor) {
  m_monitors.insert( std::move(monitor) );
  return StatusCode::SUCCESS;
}


StatusCode MonitoredRange::newEvent(const CostData& data, const float weight) {
  for (auto& monitor : getMonitors()) {
    ATH_CHECK(monitor->newEvent(data, weight));
    ATH_CHECK(monitor->endEvent());
  }
  ATH_CHECK(m_cachedLifetimeHistPtr != nullptr);
  m_cachedLifetimeHistPtr->Fill(0., data.eventLiveTime());
  return StatusCode::SUCCESS;
}
