/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprPedestalPlotManager.h
//
// Author: Patrick Rieck - rieck@physik.hu-berlin.de
//
// Special plotter for pedestal monitoring
//
// This class is derived from L1CaloPprPlotManager and implements
// the monitoring of non-signal trigger tower behaviour (pedestals).

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOPPRPEDESTALPLOTMANAGER
#define TRIGT1CALOCALIBTOOLS_L1CALOPPRPEDESTALPLOTMANAGER

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibTools/L1CaloPprPlotManager.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class ITHistSvc;
class ManagedMonitorToolBase;
class L1CaloPprConditionsContainer;
class L1CaloPprConditionsContainerRun2;

namespace LVL1 {
class IL1CaloOfflineTriggerTowerTools;
class IL1TriggerTowerTool;
class TriggerTower;
}

class L1CaloPprPedestalPlotManager : public L1CaloPprPlotManager
{
public:
	// constructor for handling offline monitoring
	L1CaloPprPedestalPlotManager(ITHistSvc* histoSvc,
	                             ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
	                             const unsigned int lumimax,
	                             const std::string& pathInRootFile);
	// constructor for online monitoring
	L1CaloPprPedestalPlotManager(ManagedMonitorToolBase* aMonObj,
	                             ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
	                             const unsigned int lumimax,
	                             const std::string& pathInRootFile);
	inline ~L1CaloPprPedestalPlotManager() {};
	inline void SetPedestalMaxWidth(double &width) { m_pedestalMaxWidth = width; };
	inline void SetDistanceFromHeadOfTrain(const std::vector<std::pair<bool, int16_t>> &distance) { m_distanceFromHeadOfTrain = distance; };


private:

	// Providing pedestal for histograms
	double getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer);
	// decide whether the pedestal
	// is actually plotted
	bool doMonitoring(double &value);

	void fillPartitionOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer, double &value);
	void fillGlobalOnlineHistos(const xAOD::TriggerTower* trigTower, CalLayerEnum layer, double &value);
        void fillDifferentialOnlineHistos(const xAOD::TriggerTower* trigTower, unsigned int &coolId, CalLayerEnum theLayer, double &value);

	ServiceHandle<L1CaloCondSvc> m_l1CondSvc;
	
	L1CaloPprConditionsContainer* m_conditionsContainer;
	
	L1CaloPprConditionsContainerRun2* m_conditionsContainerRun2;

	void loadConditionsContainer();

	double m_pedestalMaxWidth; // plotting pedestal only if for every slice
	// the deviation from the mean pedestal
	// value is less
	
	std::vector<std::pair<bool, int16_t>> m_distanceFromHeadOfTrain; //LHC bunch structure
	
	bool m_firstCall;
	
	bool m_isRun2;

};

#endif
