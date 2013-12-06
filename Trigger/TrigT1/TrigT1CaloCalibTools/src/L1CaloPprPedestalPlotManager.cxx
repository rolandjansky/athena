/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprPedestalPlotManager.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"


L1CaloPprPedestalPlotManager::L1CaloPprPedestalPlotManager(ITHistSvc* histoSvc, 
							   ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(histoSvc, 
			   offlineTT_tool, 
			   0, 
			   lumimax, 
			   pathInRootFile, 
			   "Pedestal", 
			   "pedestal", 
			   "Pedestal", 
			   "",
			   true),
      m_l1CondSvc("L1CaloCondSvc", histoSvc->name()),
      m_conditionsContainer(0),
      m_pedestalMaxWidth(0.),
      m_firstCall(true)
{
}

// --------------------------------------------------------------------------

L1CaloPprPedestalPlotManager::L1CaloPprPedestalPlotManager(ManagedMonitorToolBase* aMonObj,
							   ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(aMonObj,
			   onlineTT_tool,
			   0,
			   lumimax,
			   pathInRootFile,
			   "Pedestal",
			   "pedestal",
			   "Pedestal",
			   ""),
      m_l1CondSvc("L1CaloCondSvc", aMonObj->name()),
      m_conditionsContainer(0),
      m_pedestalMaxWidth(0.),
      m_firstCall(true)

{
}

// --------------------------------------------------------------------------

double L1CaloPprPedestalPlotManager::getMonitoringValue(const LVL1::TriggerTower* trigTower, CalLayerEnum theLayer)
{
    // only analyze non-signal towers
    // otherwise return default value -1000.
    if ( theLayer == Emlayer )
    {
        if ( trigTower->emEnergy() ) { return -1000.; }
    }
    else 
    {
        if ( trigTower->hadEnergy() ) { return -1000.; }
    }
  
    double eta = trigTower->eta();
    double phi = trigTower->phi();

    unsigned int coolID;
    
    if ( theLayer == Emlayer ) 
    {
	if(isOnline) { coolID = m_ttToolOnline->channelID(eta, phi,0).id(); }
	else { coolID = m_ttToolOffline->emCoolChannelId(trigTower); }
    }
    else
    {
	if(isOnline) { coolID = m_ttToolOnline->channelID(eta, phi,1).id(); }
	else { coolID = m_ttToolOffline->hadCoolChannelId(trigTower); }
    }
    const std::vector<int>& EtLut((theLayer == Emlayer) ? trigTower->emADC() : trigTower->hadADC());
    
    const int nSlices = EtLut.size();
    double pedMean = 0;

    if (m_firstCall) {
        this->loadConditionsContainer();
        m_firstCall = false;
    }

    if (m_conditionsContainer)
    {
        const L1CaloPprConditions* ttConditions = m_conditionsContainer->pprConditions(coolID);
	if (ttConditions)
	{
	    pedMean = ttConditions->pedMean();
	}
	else
	{
	    *m_log<<MSG::WARNING<< "No L1CaloPprConditions available" << endreq;
	}
    }
    else
    {
        *m_log<<MSG::WARNING<< "No Conditions Container available" << endreq;
    }

    double pedestal = 0;  // to be returned
    double pedestalAtSlice;
    
    // now compute monitored value:
    // average the difference between the ADC count at every slice
    // and the mean pedestal applied to the tower over the slices
    for( int i = 0; i < nSlices; i++ )
    {
        pedestalAtSlice = EtLut[i]-pedMean;
	// only analyze towers with activity close 
	// to the pedestal mean value
	if ( fabs(pedestalAtSlice) > m_pedestalMaxWidth ) { return -1000.; }
	pedestal += pedestalAtSlice;
    }
    pedestal/=static_cast<double>(nSlices);
    return pedestal;
    
}

// --------------------------------------------------------------------------

bool L1CaloPprPedestalPlotManager::doMonitoring(double &value)
{
    // check if default value (-1000.) has been determined
    return ( value!=-1000. );
}

// --------------------------------------------------------------------------

void L1CaloPprPedestalPlotManager::loadConditionsContainer()
{
    if (m_l1CondSvc)
    {
        *m_log<<MSG::DEBUG<< "Retrieving Conditions Container" << endreq;
	StatusCode sc = m_l1CondSvc->retrieve(m_conditionsContainer);
	if (sc.isFailure()) 
	{
	    *m_log<<MSG::WARNING<< "Could not retrieve Conditions Container" << endreq;
	}
	else 
	{
	    *m_log<<MSG::DEBUG<< "Retrieved Conditions Container" << endreq;
	}
    }
    else 
    {
        *m_log<<MSG::WARNING<< "Could not retrieve Conditions Containers" << endreq;
    }
}
