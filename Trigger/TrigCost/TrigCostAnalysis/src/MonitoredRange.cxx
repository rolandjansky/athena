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
  m_monitors(),
  m_seenLB()
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
    ATH_CHECK(monitor->endEvent(weight));
  }
  ATH_CHECK(m_cachedLifetimeHistPtr != nullptr);
  const bool isNewLB = m_seenLB.insert( data.lb() ).second; // .second is true if a new element was inserted
  if (data.liveTimeIsPerEvent() or isNewLB) {
    // We have two modes of operation. Either we process an EnhancedBias file where we know exactly how many events to expect
    // and can hence fill a per-event live time (meaning that the normalisation is correct, even if we do not run over all events).
    // For this case we increment this bin for event event this monitor sees.
    // Or, we can process P1 data where we only know the LB length and must assume that all events are collected and processed,
    // otherwise the normalisation will be off. For this case we increment this one for every unique LB this monitor sees.
    m_cachedLifetimeHistPtr->Fill(0., data.liveTime());
  }
  
  return StatusCode::SUCCESS;
}
