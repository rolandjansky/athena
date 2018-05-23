#include "AFP_Monitoring/AFPSiLayerSummaryManager.h"
#include "AFP_Monitoring/IAFPSiLayerSummaryEventEnd.h"
#include "AFP_Monitoring/IAFPSiLayerSummaryHit.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/AFPSiLayerSummaryProfileBase.h"


AFPSiLayerSummaryManager::~AFPSiLayerSummaryManager ()
{
  for (AFPSiLayerSummaryProfileBase* base : m_bases)
    delete base;
}

void AFPSiLayerSummaryManager::book () const
{
  for (AFPSiLayerSummaryProfileBase* baseDist : m_bases)
    baseDist->book();
}

void AFPSiLayerSummaryManager::fillHits (const std::string& binName, const xAOD::AFPSiHit& hit) const
{
  for (IAFPSiLayerSummaryHit* hitDist : m_hits)
    hitDist->fillHit(binName, hit);
}

void AFPSiLayerSummaryManager::fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) const
{
  for (IAFPSiLayerSummaryEventEnd* eventEndDist : m_eventEnds)
    eventEndDist->fillEventEnd(binName, layer);
}


// create the most common templates, which creation is prevented in the header file

// createSummaryHits
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<int> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, int (xAOD::AFPSiHit::*value)() const);
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<double> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, double (xAOD::AFPSiHit::*value)() const);
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryHits<float> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, float (xAOD::AFPSiHit::*value)() const);

// createSummaryEventEnd
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<int> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, int (IAFPSiLayerMonitor::*value)() const);
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<float> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, float (IAFPSiLayerMonitor::*value)() const);
template AFPSiLayerSummaryProfileBase* AFPSiLayerSummaryManager::createSummaryEventEnd<double> (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, double (IAFPSiLayerMonitor::*value)() const);
