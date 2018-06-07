/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTECHNICALMONITORTOOL_H
#define AFPTECHNICALMONITORTOOL_H

#include <vector>
#include <string>

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include "AFPTechnicalStationMonitor.h"

// forward declarations for lightweight histograms
class LWHist1D;

class AFPTechnicalMonitorTool : public ManagedMonitorToolBase
{
public:

  AFPTechnicalMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  virtual ~AFPTechnicalMonitorTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  ///@brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

protected:
  static const int s_cNearStationIndex;
  static const int s_cFarStationIndex;
  const std::string m_histsDirectoryName;

  AFPTechnicalStationMonitor m_cNearStation;		///< station on C side that is closer to the interaction point
  AFPTechnicalStationMonitor m_cFarStation;		///< station on C side that is further away from the interaction point
  std::vector<AFPTechnicalStationMonitor*> m_stationsMonitors;
  
  // histograms
  LWHist1D* m_bcIDAfp;		///< distribution of bcID sent by AFP
  LWHist1D* m_bcIDATLAS;		///< distribution of bcID sent by ATLAS
  LWHist1D* m_bcIDDiffAfpAtlas;		///< distribution of difference between ATLAS and AFP bcID
  
};

#endif
