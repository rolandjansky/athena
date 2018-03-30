/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPSILAYERSUMMARYMANAGER_H
#define AFPSILAYERSUMMARYMANAGER_H

#include "AFP_Monitoring/AFPSiLayerSummaryProfileHit.h"
#include "AFP_Monitoring/AFPSiLayerSummaryProfileEventEnd.h"

#include <xAODForward/AFPSiHit.h>

#include<vector>

class AFPHitsMonitorTool;
class AFPSiLayerSummaryProfileBase;
class IAFPSiLayerSummaryHit;
class IAFPSiLayerSummaryEventEnd;
class IAFPSiLayerMonitor;

class AFPSiLayerSummaryManager
{
public:
  ~AFPSiLayerSummaryManager ();

  void book () const;
  void fillHits (const std::string& binName, const xAOD::AFPSiHit& hit) const;
  void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) const;

  template<typename T>
  AFPSiLayerSummaryProfileBase* createSummaryHits (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (xAOD::AFPSiHit::*value)() const);

  template<typename T>
  AFPSiLayerSummaryProfileBase* createSummaryEventEnd (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (IAFPSiLayerMonitor::*value)() const);

private:
  std::vector<AFPSiLayerSummaryProfileBase*> m_bases;
  std::vector<IAFPSiLayerSummaryHit*> m_hits;
  std::vector<IAFPSiLayerSummaryEventEnd*> m_eventEnds;
};


template<typename T>
AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (xAOD::AFPSiHit::*value)() const)
{
  AFPSiLayerSummaryProfileHit<T>* summary = new AFPSiLayerSummaryProfileHit<T>(parentTool, monGrp, name, title, value);
  m_bases.push_back(summary);
  m_hits.push_back(summary);

  return summary;
}

// prevent creation of separete definitions for the most common templates
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<int> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, int (xAOD::AFPSiHit::*value)() const);
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<double> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, double (xAOD::AFPSiHit::*value)() const);
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<float> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, float (xAOD::AFPSiHit::*value)() const);



template<typename T>
AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (IAFPSiLayerMonitor::*value)() const)
{
  AFPSiLayerSummaryProfileEventEnd<T>* summary = new AFPSiLayerSummaryProfileEventEnd<T>(parentTool, monGrp, name, title, value);
  m_bases.push_back(summary);
  m_eventEnds.push_back(summary);

  return summary;
}

// prevent creation of separete definitions for the most common templates
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<int> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, int (IAFPSiLayerMonitor::*value)() const);
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<float> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, float (IAFPSiLayerMonitor::*value)() const);
extern template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<double> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, double (IAFPSiLayerMonitor::*value)() const);


#endif
