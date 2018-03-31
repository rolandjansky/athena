/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPHITSMONITORTOOL_H
#define AFPHITSMONITORTOOL_H

#include "AFP_Monitoring/IAFPSiStationMonitor.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include <vector>
#include <string>

class AFPSiLayerSummaryManager;

class AFPHitsMonitorTool : public ManagedMonitorToolBase
{
public:

  AFPHitsMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  virtual ~AFPHitsMonitorTool();

  StatusCode initialize() override;
  
  StatusCode bookHistogramsRecurrent() override;
  StatusCode bookHistograms() override;
  StatusCode fillHistograms() override;
  StatusCode procHistograms() override;

  ///@brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

  const ToolHandleArray<IAFPSiStationMonitor>* stationsMonitors () const {return &m_stationsMonitors;}

  AFPSiLayerSummaryManager* summaryManager () const {return m_summaryManager;}
  
  double pileUp () const {return m_lumiBlockMuTool->averageInteractionsPerCrossing();}

protected:
  const std::string m_histsDirectoryName; ///< name of directory for all histograms

  ToolHandleArray<IAFPSiStationMonitor> m_stationsMonitors;

  AFPSiLayerSummaryManager* m_summaryManager;

  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;
  
  void makeLayerSummaryHist (const std::string inputHistName, const std::string outputHistName, const std::string outputHistTitle);
};

#endif
