/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprPedestalCorrectionPlotManager.h
//
// Author: Patrick Rieck - rieck@physik.hu-berlin.de
//
// Special plotter for pedestal correction monitoring
// 
// This class is derived from L1CaloPprPlotManager and implements
// the monitoring of the pedestal correction.

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOPPRPEDESTALCORRECTIONPLOTMANAGER
#define TRIGT1CALOCALIBTOOLS_L1CALOPPRPEDESTALCORRECTIONPLOTMANAGER

#include <string>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibTools/L1CaloPprPlotManager.h"

#include "xAODTrigL1Calo/xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/xAODTrigL1Calo/TriggerTowerContainer.h"

class ITHistSvc;
class ManagedMonitorToolBase;
class L1CaloPprConditionsContainer;

class TrigT1CaloLWHistogramTool;

namespace LVL1 {
    class IL1CaloOfflineTriggerTowerTools;
    class IL1TriggerTowerTool;
    class TriggerTower;
}

class L1CaloPprPedestalCorrectionPlotManager : public L1CaloPprPlotManager
{
    public:
        // constructor for handling offline monitoring
        L1CaloPprPedestalCorrectionPlotManager(ITHistSvc* histoSvc, 
				     ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
				     const unsigned int lumimax,
				     const std::string& pathInRootFile);
        // constructor for online monitoring
	L1CaloPprPedestalCorrectionPlotManager(ManagedMonitorToolBase* aMonObj,
				     ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
				     const unsigned int lumimax,
				     const std::string& pathInRootFile);
	inline ~L1CaloPprPedestalCorrectionPlotManager() {};
// 	inline void SetPedestalMaxWidth(double &width) { m_pedestalMaxWidth = width; };

    private:
		
	// Providing pedestal for histograms
	double getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer);
	// decide whether the pedestal correction
	// is actually plotted
	bool doMonitoring(double &value);
	
	void fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value);
	void fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value);
	
	ServiceHandle<L1CaloCondSvc> m_l1CondSvc;

        bool m_firstCall;

};

#endif
