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

/// Class managing (booking and filling) summary distributions
class AFPSiLayerSummaryManager
{
public:
  /// Deletes all summary objects, i.e. all in AFPSiLayerSummaryManager::m_bases
  ~AFPSiLayerSummaryManager ();

  /// Executes AFPSiLayerSummaryProfileBase::book() method on all objects in AFPSiLayerSummaryManager::m_bases
  void book () const;

  /// Executes IAFPSiLayerSummaryHit::fillHits() on all objects in AFPSiLayerSummaryManager::m_hits
  void fillHits (const std::string& binName, const xAOD::AFPSiHit& hit) const;

  /// Executes IAFPSiLayerSummaryHit::fillEventEnd() on all objects in AFPSiLayerSummaryManager::m_eventEnds
  void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) const;


  /// @brief Creates a new monitoring object of type
  /// AFPSiLayerSummaryProfileHit adds to AFPSiLayerSummaryManager::m_bases and AFPSiLayerSummaryManager::m_hits.
  ///
  /// Created object will be filled with every hit. The filling is
  /// done in the layer object AFPSiLayerMonitor::fillHistograms()
  /// 
  /// @param parentTool pointer to the main monitoring tool to which the summary distribution belongs to
  /// @param monGrp monitoring properties e.g. frequency of rebooking (every lumi-block or run), directory where to save the distribution
  /// @param name name of the distribution will be used as key in the output file
  /// @param title title of the distribution will be displayed at the top
  /// @param value pointer to the member method calculating the value for which distribution is to be created
  /// 
  /// @return pointer to the base class of created object in order to avoid templates
  template<typename T>
  AFPSiLayerSummaryProfileBase* createSummaryHits (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (xAOD::AFPSiHit::*value)() const);

  /// @brief Creates a new monitoring object of type AFPSiLayerSummaryProfileEventEnd adds to AFPSiLayerSummaryManager::m_bases and AFPSiLayerSummaryManager::m_eventEnds.
  ///
  /// Created object will be filled once per event for each layer. The
  /// filling is done in the layer object
  /// AFPSiLayerMonitor::eventEnd()
  /// 
  /// @param parentTool pointer to the main monitoring tool to which the summary distribution belongs to
  /// @param monGrp monitoring properties e.g. frequency of rebooking (every lumi-block or run), directory where to save the distribution
  /// @param name name of the distribution will be used as key in the output file
  /// @param title title of the distribution will be displayed at the top
  /// @param value pointer to the member method calculating the value for which distribution is to be created
  /// 
  /// @return pointer to the base class of created object in order to avoid templates
  template<typename T>
  AFPSiLayerSummaryProfileBase* createSummaryEventEnd (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (IAFPSiLayerMonitor::*value)() const);

private:
  /// Vector of all summary objects.
  std::vector<AFPSiLayerSummaryProfileBase*> m_bases;

  /// Vector of summary objects which should be filled for every hit.
  std::vector<IAFPSiLayerSummaryHit*> m_hits;

  /// Vector of summary objects which should be filled once per event for every plane.
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
