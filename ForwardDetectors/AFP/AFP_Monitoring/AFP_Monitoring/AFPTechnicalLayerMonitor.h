/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPTECHNICALLAYERMONITOR_H
#define AFP_MONITORING_AFPTECHNICALLAYERMONITOR_H

#include <list>

#include <AFP_RawEv/AFP_SiRawData.h>

// forward declarations for lightweight histograms
class LWHist1D;
class LWHist2D;

class ManagedMonitorToolBase;

class AFPTechnicalLayerMonitor
{
public:
  AFPTechnicalLayerMonitor(const int pixelLayerID, const int stationID);
  ~AFPTechnicalLayerMonitor();

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "");
  void fillHistograms(const AFP_SiRawData& hit);
  void eventEnd();		///< method that should be called when event processing is finished

  void endOfLumiBlock(ManagedMonitorToolBase* toolToStoreHistograms); ///< Process histograms at the end of lumiblock

protected:
  std::string makeHistName (const std::string name) const; ///< create a name suffixed with station and layer numbers
  std::string makeHistTitle (const std::string title) const; ///< create a title suffixed with station and layer numbers

  // internal variables
  const int m_pixelLayerID;
  const int m_stationID;

  // histograms
};

#endif
