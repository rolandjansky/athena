/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprEtCorrelationPlotManager.h
//
// Author: Patrick Rieck - rieck@physik.hu-berlin.de
//
// Special plotter for Et correlation monitoring
//
// This class is derived from L1CaloPprPlotManager and implements
// the monitoring of the correlation between Et as measured by
// L1Calo and the Et reconstructed using the full granularity
// of the calorimeter. Actually what's plotted is simply the ratio
// of the two Et values.
//
#ifndef TRIGT1CALOCALIBTOOLS_L1CALOPPRETCORRELATIONPLOTMANAGER
#define TRIGT1CALOCALIBTOOLS_L1CALOPPRETCORRELATIONPLOTMANAGER

#include <map>
#include <string>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloMonitoringCaloTool.h"
#include "TrigT1CaloCalibTools/L1CaloPprPlotManager.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class ITHistSvc;
class ManagedMonitorToolBase;
class Identifier;
class StoreGateSvc;
class StatusCode;

namespace LVL1 {
class IL1CaloOfflineTriggerTowerTools;
class IL1TriggerTowerTool;
class TriggerTower;
}

class L1CaloPprEtCorrelationPlotManager : public L1CaloPprPlotManager
{
public:
	// constructor for handling offline monitoring
	L1CaloPprEtCorrelationPlotManager(ITHistSvc* histoSvc,
	                                  ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
	                                  const unsigned int lumimax,
	                                  const std::string& pathInRootFile);
	// constructor for online monitoring
	L1CaloPprEtCorrelationPlotManager(ManagedMonitorToolBase* aMonObj,
	                                  ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
	                                  const unsigned int lumimax,
	                                  const std::string& pathInRootFile);
	inline ~L1CaloPprEtCorrelationPlotManager() {};

	inline void SetCaloCellContainer(std::string name) {m_caloCellContainerName = name;}

	// retrieve calo cell container for offline monitoring
	// for online monitoring set map of cell energies for every tower
	StatusCode getCaloCells();

	inline void SetEtMin(double et) { m_EtMin = et; }

private:

	ServiceHandle<StoreGateSvc> m_storeGate;
	std::string m_caloCellContainerName;
	ToolHandle<LVL1::IL1CaloMonitoringCaloTool> m_caloTool;
	// threshold for L1Calo and calo cell energies
	double m_EtMin;

	// Providing Et correlation (=Et ratio)
	double getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer);

	// decide whether the correlation
	// is acctually plotted
	bool doMonitoring(double &value);

	void loadTools();

};

#endif
