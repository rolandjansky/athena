/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloMonitoring/PPrStabilityMon.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibTools/L1CaloPprFineTimePlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprPedestalPlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprPedestalCorrectionPlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprEtCorrelationPlotManager.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"

PPrStabilityMon::PPrStabilityMon(const std::string & type, const std::string & name, const IInterface* parent): ManagedMonitorToolBase ( type, name, parent ),
    m_ppmADCMinValue(0),
    m_lumiBlockMax(0),
    m_errorTool("TrigT1CaloMonErrorTool"),
    m_histTool("TrigT1CaloLWHistogramTool"),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_fineTimePlotManager(0),
    m_pedestalPlotManager(0),
    m_pedestalCorrectionPlotManager(0),
    m_etCorrelationPlotManager(0),
    m_doFineTimeMonitoring(0),
    m_doPedestalMonitoring(0),
    m_doPedestalCorrectionMonitoring(0),
    m_doEtCorrelationMonitoring(0),
    m_evtInfo(0),
    m_fineTimeCut(0),
    m_pedestalMaxWidth(0),
    m_EtMinForEtCorrelation(0)
{
  declareProperty("BS_TriggerTowerContainer", m_TriggerTowerContainerName = "xAODTriggerTowers");
  declareProperty("BS_xAODTriggerTowerContainer", m_xAODTriggerTowerContainerName = "xAODTriggerTowers");
  declareProperty("ppmADCMinValue", m_ppmADCMinValue=60,
                  "Cut on ADC minimum value, used for fine time monitoring");
  declareProperty("ppmADCMaxValue", m_ppmADCMaxValue=1023,
                  "Cut on ADC maximum value, used for fine time monitoring"); 
  declareProperty("PathInRootFile", m_PathInRootFile="L1Calo/PPrStabilityMon");
  declareProperty("doFineTimeMonitoring", m_doFineTimeMonitoring = true );
  declareProperty("doPedestalMonitoring", m_doPedestalMonitoring = true );
  declareProperty("doPedestalCorrectionMonitoring", m_doPedestalCorrectionMonitoring = true );
  declareProperty("doEtCorrelationMonitoring", m_doEtCorrelationMonitoring = true );
  declareProperty("lumiMax", m_lumiBlockMax = 2000,
                  "Maximum number of lumiblocks in stability plots");
  declareProperty("fineTimeCut", m_fineTimeCut = 20,
                  "Maximum acceptable fine time value");
  declareProperty("pedestalMaxWidth", m_pedestalMaxWidth = 10,
                  "Maximum acceptable pedestal deviation from mean");
  declareProperty("EtMinForEtCorrelation", m_EtMinForEtCorrelation=5,
                  "Minimum Et cut for Et correlation");
  declareProperty("doCaloQualCut", m_doCaloQualCut=true,
                  "Switch for calo quality cut via job options in fine time");
}

PPrStabilityMon::~PPrStabilityMon()
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode PPrStabilityMon::initialize()
/*---------------------------------------------------------*/
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version "
		   << PACKAGE_VERSION << endreq;
    
    StatusCode sc;
      std::cout << "0 m_TriggerTowerContainerName = " << m_TriggerTowerContainerName << std::cout;
    sc = ManagedMonitorToolBase::initialize();
    if (sc.isFailure()) return sc;
    
    sc = m_errorTool.retrieve();
    if( sc.isFailure() ){msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool" << endreq;return sc;}
    
    sc = m_histTool.retrieve();
    if( sc.isFailure() ) {msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool" << endreq; return sc;}
    
    sc = m_ttTool.retrieve();
    if( sc.isFailure() ) {msg(MSG::ERROR) << "Unable to locate Tool L1TriggerTowerTool" << endreq;return sc;}
        
    if (m_doFineTimeMonitoring)
    {
        m_fineTimePlotManager = new L1CaloPprFineTimePlotManager(this,
								m_ttTool,
								m_ppmADCMinValue,
								m_lumiBlockMax,
								m_PathInRootFile+"/FineTime");
	m_fineTimePlotManager->SetFineTimeCut(m_fineTimeCut);
	m_fineTimePlotManager->SetPpmAdcMaxValue(m_ppmADCMaxValue); 
	m_fineTimePlotManager->EnableCaloQualCut(m_doCaloQualCut);
    }
    if (m_doPedestalMonitoring)
    {
        m_pedestalPlotManager = new L1CaloPprPedestalPlotManager(this,
								m_ttTool,
								m_lumiBlockMax,
								m_PathInRootFile+"/Pedestal");
	m_pedestalPlotManager->SetPedestalMaxWidth(m_pedestalMaxWidth);
    }
    if (m_doPedestalCorrectionMonitoring)
    {
        m_pedestalCorrectionPlotManager = new L1CaloPprPedestalCorrectionPlotManager(this,
								m_ttTool,
								m_lumiBlockMax,
								m_PathInRootFile+"/PedestalCorrection");
    }
    if (m_doEtCorrelationMonitoring){
        m_etCorrelationPlotManager = new L1CaloPprEtCorrelationPlotManager(this,
								m_ttTool,
								m_lumiBlockMax,
								m_PathInRootFile+"/EtCorrelation");
	m_etCorrelationPlotManager->SetEtMin(m_EtMinForEtCorrelation);
    }
    
    return StatusCode::SUCCESS;
    
}

StatusCode PPrStabilityMon::finalize()
{
    if (m_doFineTimeMonitoring) {delete m_fineTimePlotManager;}
    if (m_doPedestalMonitoring) {delete m_pedestalPlotManager;}
    if (m_doPedestalCorrectionMonitoring) {delete m_pedestalCorrectionPlotManager;}
    if (m_doEtCorrelationMonitoring) {delete m_etCorrelationPlotManager;}
    return StatusCode::SUCCESS;
}

StatusCode PPrStabilityMon::fillHistograms()
{
    const bool debug = msgLvl(MSG::DEBUG);

    // Skip events believed to be corrupt
    if (m_errorTool->corrupt()){if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;return StatusCode::SUCCESS;}

    StatusCode sc;

    //Retrieve eventInfo from storeGate;
    m_evtInfo = 0;
    sc = evtStore()->retrieve(m_evtInfo);
    if( sc.isFailure() ) { msg(MSG::ERROR) <<"Could not retrieve Event Info" <<endreq; return sc;}

    //Retrieve TriggerTowers from SG
    const xAOD::TriggerTowerContainer* trigTwrColl = 0; 
    sc = evtStore()->retrieve(trigTwrColl,m_xAODTriggerTowerContainerName);
    if (sc.isFailure())
    {
        if (debug) msg(MSG::DEBUG) << "No TriggerTower found at "<< m_xAODTriggerTowerContainerName << endreq ;
        return sc;
    }
    if (debug) msg(MSG::DEBUG)<<"In Fill histograms"<<endreq;
    
    if (m_doEtCorrelationMonitoring) {
        sc = m_etCorrelationPlotManager->getCaloCells();
	if (sc.isFailure()) return sc;
    }
    
    //load the CaloCells and the Reference Value folder from COOL (or an sqlite file) in case you want to monitor the fine time
    if (m_doFineTimeMonitoring){
       //load the standalone sqlite db containing the fine time references and calibration values
      sc = m_ttTool->loadFTRefs();
      if (sc.isFailure()) {msg(MSG::WARNING) << "Failed to load FineTimeReference Folder"<< endreq; return sc;}
      //if you want to use the calo quality cut also load the calo cell container
      if(m_doCaloQualCut){
	StatusCode sc2 = m_fineTimePlotManager->getCaloCells();
	if (sc2.isFailure()) {msg(MSG::WARNING) << "Failed to load Calo Cells"<< endreq; return sc2;}
      }
    }
    
    // ================= Container: TriggerTower ===========================
    
    xAOD::TriggerTowerContainer::const_iterator TriggerTowerIterator = trigTwrColl->begin(); 
    xAOD::TriggerTowerContainer::const_iterator TriggerTowerIteratorEnd = trigTwrColl->end(); 
    
    for (; TriggerTowerIterator != TriggerTowerIteratorEnd; ++TriggerTowerIterator) 
    {
        const double eta = (*TriggerTowerIterator)->eta();
        const double phi = (*TriggerTowerIterator)->phi();

        const L1CaloCoolChannelId CoolChannelID = m_ttTool->channelID(eta,phi,(*TriggerTowerIterator)->layer());
	
        bool Dead = m_ttTool->disabledChannel(CoolChannelID);
		
        if (m_doFineTimeMonitoring) {
            // Need signal
	  int Peak = (*TriggerTowerIterator)->adcPeak();
	  unsigned int PeakVal = (*TriggerTowerIterator)->adc()[Peak];
	  if (PeakVal > m_ppmADCMinValue) {
	    //Set the reference and calibration values for the fine time, they are stored per cool ID in a data base
	      std::pair<double, double> Ref = m_ttTool->refValues(CoolChannelID);
	      double Reference = Ref.first;
	      double CalFactor = Ref.second;

	      m_fineTimePlotManager->SetReferenceValue(Reference);
	      m_fineTimePlotManager->SetCalibrationFactor(CalFactor);
	      m_fineTimePlotManager->Analyze(m_evtInfo, *TriggerTowerIterator, Dead);
	   }
	}

	const int cpEt = (*TriggerTowerIterator)->cpET();
	const int jepEt = (*TriggerTowerIterator)->jepET();

	if (m_doPedestalMonitoring) {
	    // Need no signal
	    if (cpEt == 0 && jepEt == 0) {  //Hanno: changed || to &&
	        m_pedestalPlotManager->Analyze(m_evtInfo, *TriggerTowerIterator, Dead);
	    }
	}
	if (m_doPedestalCorrectionMonitoring) {
	        m_pedestalCorrectionPlotManager->Analyze(m_evtInfo, *TriggerTowerIterator, Dead);
	}
	if (m_doEtCorrelationMonitoring) {	  
	    // Need signal
	    if (cpEt > m_EtMinForEtCorrelation || jepEt > m_EtMinForEtCorrelation) {
	        m_etCorrelationPlotManager->Analyze(m_evtInfo, *TriggerTowerIterator, Dead);
	    }
	}
    }
    
    return sc;
}

StatusCode PPrStabilityMon::procHistograms()
{
    if(endOfRun){}
    return StatusCode::SUCCESS;
}

