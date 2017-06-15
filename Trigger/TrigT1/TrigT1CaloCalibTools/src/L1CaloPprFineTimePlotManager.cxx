/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprFineTimePlotManager.h"

#include "GaudiKernel/ITHistSvc.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTower.h"


L1CaloPprFineTimePlotManager::L1CaloPprFineTimePlotManager(ITHistSvc* histoSvc, 
							   ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
							   const int ADC_cut,
							   const unsigned int lumimax,
							   const std::string& pathInRootFile)
    : L1CaloPprPlotManager(histoSvc, 
			   offlineTT_tool, 
			   ADC_cut, 
			   lumimax, 
			   pathInRootFile, 
			   "FineTime", 
			   "fineTime", 
			   "Fine Time", 
			   "[ns]",
			   true),
      m_storeGate("StoreGateSvc", histoSvc->name()),
      m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool"),
      m_fineTimeCut(0),
      m_ppmAdcMaxValue(1023),     
      m_RefValue(0),
      m_emRefValue(0.),
      m_hadRefValue(0.),
      m_CalFactor(0),
      m_emCalFactor(0.),
      m_hadCalFactor(0.),
      m_doCaloQualCut(true)
{
    this->loadTools();

}

// --------------------------------------------------------------------------

L1CaloPprFineTimePlotManager::L1CaloPprFineTimePlotManager(ManagedMonitorToolBase* aMonObj,
				     ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
				     const int ADC_cut, const unsigned int lumimax,
				     const std::string& pathInRootFile)
    : L1CaloPprPlotManager(aMonObj,
			   onlineTT_tool,
			   ADC_cut,
			   lumimax,
			   pathInRootFile,
			   "FineTime",
			   "fineTime",
			   "Fine Time",
			   "[ns]"),
      m_storeGate("StoreGateSvc", aMonObj->name()),
      m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool"),
      m_fineTimeCut(0),
      m_ppmAdcMaxValue(1023),     
      m_RefValue(0),
      m_emRefValue(0.),
      m_hadRefValue(0.),
      m_CalFactor(0),
      m_emCalFactor(0.),
      m_hadCalFactor(0.),
      m_doCaloQualCut(true)
{
    this->loadTools();

}

// --------------------------------------------------------------------------

StatusCode L1CaloPprFineTimePlotManager::getCaloCells()
{  
    StatusCode sc;
    
    sc = m_caloTool->loadCaloCells();
    if ( sc.isFailure() ) {
        *m_log<<MSG::WARNING<<"Could not load CaloCells" << endmsg;
    }

    return sc;
    
}

// --------------------------------------------------------------------------


float L1CaloPprFineTimePlotManager::LArQuality(const xAOD::TriggerTower* trigTower, int layer){//layer: 0 is for the EMLayer, 1 is for HAD
  
    Identifier id(0);

    if(m_isOnline){
      id = m_ttToolOnline->identifier(trigTower->eta(), trigTower->phi(), layer); 
    }else{
      if(layer == 0) id = m_ttToolOffline->emID(trigTower->eta(),trigTower->phi());
      else if(layer == 1) id = m_ttToolOffline->hadID(trigTower->eta(),trigTower->phi());
    }

    float quality = m_caloTool->caloQuality(id);
   
    return quality;
    
}
// --------------------------------------------------------------------------
bool  L1CaloPprFineTimePlotManager::badLArQuality(float qual){
   
    if((qual >= 0) && (qual < 20000)){
      return false;
    }
    
    return true;
  
}
// --------------------------------------------------------------------------
bool  L1CaloPprFineTimePlotManager::badHecQuality(float qual){
   
    if((qual >= 0) && (qual < 5000)){
      return false;
    }
    
    return true;
  
}
// --------------------------------------------------------------------------

double L1CaloPprFineTimePlotManager::getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer)
{
    double fineTime = -1000.; // default value not supposed to be filled in histograms
    
    // Use reference rather than copying ADC vector
    const std::vector<short unsigned int>& ADCslices = trigTower->adc();
    unsigned int ADCPeakIndex = trigTower->adcPeak();
    if (ADCslices[ADCPeakIndex] <= m_ppmAdcMinValue) return fineTime;
    
    bool isSaturatedTT = true;
    bool badCaloQuality = false;
    double refValue = 0;
    double calFactor = 0;    

    if ( theLayer == Emlayer )
    {
	refValue = m_RefValue;
	calFactor = m_CalFactor;
	isSaturatedTT = trigTower->isCpSaturated();
	if(m_doCaloQualCut) badCaloQuality = this->badLArQuality(this->LArQuality(trigTower,0)); 
    }
    else //had Layer
    {
	refValue = m_RefValue;
	calFactor = m_CalFactor;
	isSaturatedTT = trigTower->isCpSaturated();
	if(m_doCaloQualCut){
	  if(fabs(trigTower->eta()) >= 3.2){
	    badCaloQuality = this->badLArQuality(this->LArQuality(trigTower,1)); 
	  }else if(fabs(trigTower->eta()) >= 1.5){
	    badCaloQuality = this->badHecQuality(this->LArQuality(trigTower,1)); 
	  }else{
	    badCaloQuality = false;
	  }
	}
    }
    //Cut on saturated Towers
    if(isSaturatedTT) return fineTime;
    
    //Cut on out LArg noise bursts
    if(badCaloQuality) return fineTime;

    unsigned int size = ADCslices.size();
    
    // the ansatz of computing fine time only makes sense
    // if there are enough adc slices
    if ( size<3 ) return fineTime;

    // Find fineTime only if the peak index is not on the boundaries
    if( ADCPeakIndex > (size - 2) || ADCPeakIndex < 1 ) return fineTime;
    
    int peakSlice     = ADCslices[ADCPeakIndex];
    int inferiorSlice = ADCslices[ADCPeakIndex - 1];
    int superiorSlice = ADCslices[ADCPeakIndex + 1];
    
    if( peakSlice > m_ppmAdcMinValue && peakSlice     < m_ppmAdcMaxValue
                                     && inferiorSlice < m_ppmAdcMaxValue
				     && superiorSlice < m_ppmAdcMaxValue )
    {
        double denom = (2*(2*peakSlice - superiorSlice - inferiorSlice));
	double numer = superiorSlice - inferiorSlice;
	if(denom > 0)
	{
	    fineTime = numer/denom;
	    // scale back to sampling interval 
	    fineTime = fineTime*m_sampleInterval;
	    // subtract the reference value
	    fineTime = fineTime - refValue;
	    if(calFactor <= 0.000001){//setting unreasonable calibration factors to minus 1
// 	       *m_log<<MSG::INFO<<"Calibration factor < 10^-10 provided! Setting it to -1" <<endmsg;
	       calFactor = -1;
	    }
	    // and calibrate the value
 	    fineTime = (-1)*fineTime/calFactor;
	}
	
    }
    return fineTime;
}

// --------------------------------------------------------------------------

bool L1CaloPprFineTimePlotManager::doMonitoring(double &value)
{
    // fine time set to -1000. by default in getFineTime()
    return ( (value != -1000.) && (fabs(value) < m_fineTimeCut) );
}

// --------------------------------------------------------------------------

void L1CaloPprFineTimePlotManager::loadTools()
{
    StatusCode sc;
    
    if (m_isOnline) {
        sc = m_caloTool.retrieve();
	if ( sc.isFailure()) {
	    *m_log<<MSG::WARNING<<"Unable to locate tool L1CaloMonitoringCaloTool" << endmsg;
	}
    }
    else {
        sc = m_storeGate.retrieve();
	if ( sc.isFailure()) {
	    *m_log<<MSG::WARNING<<"Unable to retrieve store gate" << endmsg;
	}
    }
    
    
}
