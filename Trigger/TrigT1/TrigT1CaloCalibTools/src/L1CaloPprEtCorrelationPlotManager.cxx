/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprEtCorrelationPlotManager.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"

#include <utility>

#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"


L1CaloPprEtCorrelationPlotManager::L1CaloPprEtCorrelationPlotManager(ITHistSvc* histoSvc,
								     ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
								     const unsigned int lumimax,
								     const std::string& pathInRootFile)
    : L1CaloPprPlotManager(histoSvc, 
			   offlineTT_tool, 
			   0, 
			   lumimax, 
			   pathInRootFile, 
			   "EtCaloL1Correlation", 
			   "etCorrelation", 
			   "EtCaloL1Correlation", 
			   "",
			   true),
      m_storeGate("StoreGateSvc", histoSvc->name()),
      m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool"),
      m_EtMin(0.)
{
    this->loadTools();
}

// --------------------------------------------------------------------------

L1CaloPprEtCorrelationPlotManager::L1CaloPprEtCorrelationPlotManager(ManagedMonitorToolBase* aMonObj,
							   ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(aMonObj,
			   onlineTT_tool,
			   0,
			   lumimax,
			   pathInRootFile,
			   "EtCaloL1Correlation",
			   "etCorrelation",
			   "EtCaloL1Correlation",
			   ""),
      m_storeGate("StoreGateSvc", aMonObj->name()),
      m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool"),
      m_EtMin(0.)
{
    this->loadTools();
}

// --------------------------------------------------------------------------

StatusCode L1CaloPprEtCorrelationPlotManager::getCaloCells()
{  
    StatusCode sc;
    
    const CaloCellContainer* caloCellContainer = 0;
    
    if (!isOnline){
      
        sc = m_storeGate->retrieve(caloCellContainer, m_caloCellContainerName);
	if ( sc.isFailure() || !caloCellContainer) {
	    *m_log<<MSG::WARNING<<"Could not retrieve calo cell container" << endreq;
	}
	else {
	    m_ttToolOffline->caloCells(caloCellContainer);
	}
	
    }
    else {

        sc = m_caloTool->loadCaloCells();
	if ( sc.isFailure() ) {
	    *m_log<<MSG::WARNING<<"Could not load CaloCells" << endreq;
        }

    }

    return sc;
    
}

// --------------------------------------------------------------------------

double L1CaloPprEtCorrelationPlotManager::getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer)
{
    Identifier id;
    double caloEnergy;
    double ttCpEnergy;
    ttCpEnergy=trigTower->cpET();
    if (ttCpEnergy <= m_EtMin) return -1000.;
    if(isOnline) { 
	id = m_ttToolOnline->identifier(trigTower->eta(),trigTower->phi(),trigTower->layer());
	caloEnergy = m_caloTool->et(id);
    }
    else {
      caloEnergy = m_ttToolOffline->TTCellsEt(trigTower); //update to xAOD
    }
    // round calo energy to nearest GeV in order to compare with L1 energy
    caloEnergy=int(caloEnergy+0.5);
    // set calo energy to saturation limit if necessary
    if (caloEnergy>255.) { caloEnergy=255.; }
    
    // fill histograms only if L1 and calo cell energies 
    // are above the threshold defined by m_EtMin
    if (  caloEnergy > m_EtMin && ttCpEnergy > m_EtMin ) {
        return ttCpEnergy/caloEnergy;
    }
    else {
        // return default value
        return -1000.;
    }
}

// --------------------------------------------------------------------------

bool L1CaloPprEtCorrelationPlotManager::doMonitoring(double &value)
{
    // check if default value (-1000.) has been determined
    return ( value!=-1000. );
}

// --------------------------------------------------------------------------

void L1CaloPprEtCorrelationPlotManager::loadTools()
{
    StatusCode sc;
    
    if (isOnline) {
        sc = m_caloTool.retrieve();
	if ( sc.isFailure()) {
	    *m_log<<MSG::WARNING<<"Unable to locate tool L1CaloMonitoringCaloTool" << endreq;
	}
    }
    else {
        sc = m_storeGate.retrieve();
	if ( sc.isFailure()) {
	    *m_log<<MSG::WARNING<<"Unable to retrieve store gate" << endreq;
	}
    }
    
    
}
