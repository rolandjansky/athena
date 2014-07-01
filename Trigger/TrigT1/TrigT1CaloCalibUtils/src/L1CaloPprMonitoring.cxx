/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloPprMonitoring.h"

#include "CaloEvent/CaloCellContainer.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"

L1CaloPprMonitoring::L1CaloPprMonitoring(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator),
    m_eventInfo(0),
    m_dbPpmDeadChannels(0),
    m_dbPpmDisabledTowers(0),
    m_dbFineTimeRefsTowers(0),
    m_triggerTowers(0),
    m_histoSvc(0),
    m_fineTimePlotManager(0),
    m_pedestalPlotManager(0),
    m_etCorrelationPlotManager(0),
    m_storeGate("StoreGateSvc", name),
    m_towerTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_dbPpmDeadChannelsFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"),
    m_dbPpmDisabledTowersFolder("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"),
    m_dbFineTimeRefsFolder("/TRIGGER/L1Calo/V1/References/FineTimeReferences"),
    m_PpmAdcMinValue(0),
    m_lumiBlockMax(0),
    m_doFineTimePlots(0),
    m_doPedestalPlots(0),
    m_doEtCorrelationPlots(0),
    m_fineTimeCut(0),
    m_pedestalMaxWidth(0),
    m_caloCellContainerName(""),
    m_EtMinForEtCorrelation(0)									      
{
    declareProperty("TriggerTowersLocation",m_triggerTowersLocation="TriggerTowers");
    declareProperty("ppmADCMinValue",m_PpmAdcMinValue=60);
    declareProperty("ppmADCMaxValue",m_PpmAdcMaxValue=1023); 
    declareProperty("lumiBlockMax",m_lumiBlockMax=2000);
    declareProperty("doFineTimePlots",m_doFineTimePlots=true);
    declareProperty("doPedestalPlots",m_doPedestalPlots=true);
    declareProperty("doEtCorrelationPlots",m_doEtCorrelationPlots=true);
    declareProperty("fineTimeCut",m_fineTimeCut=20);
    declareProperty("pedestalMaxWidth",m_pedestalMaxWidth=10);
    declareProperty("caloCellContainerName",m_caloCellContainerName="AllCalo");
    declareProperty("EtMinForEtCorrelation",m_EtMinForEtCorrelation=5);
    declareProperty("doCaloQualCut",m_doCaloQualCut=true);
    TH1::AddDirectory(0);
}

L1CaloPprMonitoring::~L1CaloPprMonitoring()
{}

StatusCode L1CaloPprMonitoring::initialize()
{
    StatusCode sc;
    
    sc = this->loadInTools();
    if(sc.isFailure()){return sc;}

    if (m_doFineTimePlots)
    {
        m_fineTimePlotManager = new L1CaloPprFineTimePlotManager(m_histoSvc,
								 m_towerTools,
								 m_PpmAdcMinValue,
								 m_lumiBlockMax,
								 "L1Calo/PPM");
	m_fineTimePlotManager->SetFineTimeCut(m_fineTimeCut);
	m_fineTimePlotManager->SetPpmAdcMaxValue(m_PpmAdcMaxValue); 
	m_fineTimePlotManager->EnableCaloQualCut(m_doCaloQualCut);
    }
    if (m_doPedestalPlots)
    {
        m_pedestalPlotManager = new L1CaloPprPedestalPlotManager(m_histoSvc,
								 m_towerTools,
								 m_lumiBlockMax,
								 "L1Calo/PPM");
	m_pedestalPlotManager->SetPedestalMaxWidth(m_pedestalMaxWidth);
    }
    if (m_doEtCorrelationPlots){
        m_etCorrelationPlotManager = new L1CaloPprEtCorrelationPlotManager(m_histoSvc,
									   m_towerTools,
									   m_lumiBlockMax,
									   "L1Calo/PPM");
	m_etCorrelationPlotManager->SetCaloCellContainer(m_caloCellContainerName);
	m_etCorrelationPlotManager->SetEtMin(m_EtMinForEtCorrelation);
    }
    return sc;
}

StatusCode L1CaloPprMonitoring::execute()
{
  
    StatusCode sc;

    sc = this->loadContainers();
    if(sc.isFailure()){return sc;}
    
    TriggerTowerCollection::const_iterator TT_itr;

    for(TT_itr = m_triggerTowers->begin(); TT_itr != m_triggerTowers->end(); ++TT_itr)
    {       
        //Dead Channel DB folder used for 2010 data
        //Disabled tower DB folder used for 2011/12 data
        
        const coral::AttributeList* emDbDead = 0;
        const coral::AttributeList* hadDbDead = 0;

        bool EmIsDisabled(false),HadIsDisabled(false);
        if(m_eventInfo->event_ID()->run_number() < 175000)
        {
            emDbDead    = m_towerTools->emDbAttributes(*TT_itr,m_dbPpmDeadChannels);
            hadDbDead   = m_towerTools->hadDbAttributes(*TT_itr,m_dbPpmDeadChannels);

            if(emDbDead !=0){EmIsDisabled =  true;}
            if(hadDbDead!=0){HadIsDisabled = true;}
        }
        else //2011/12 data 
        {
            emDbDead    = m_towerTools->emDbAttributes(*TT_itr,m_dbPpmDisabledTowers);
            hadDbDead   = m_towerTools->hadDbAttributes(*TT_itr,m_dbPpmDisabledTowers);

            if(m_towerTools->DisabledTower(emDbDead)){EmIsDisabled= true; }
            if(m_towerTools->DisabledTower(hadDbDead)){HadIsDisabled= true; }
	    
        }
	if (m_doFineTimePlots)
	{
	    //Set the reference and calibration values for the fine time, they are stored per cool ID in a data base
	    double emReference = 0;
	    double emCalFactor = 0;
	    double hadReference = 0;
	    double hadCalFactor = 0;
	    unsigned int emCoolId = m_towerTools->emCoolChannelId(*TT_itr);
	    unsigned int hadCoolId = m_towerTools->hadCoolChannelId(*TT_itr);
	    CondAttrListCollection::const_iterator emItr = m_dbFineTimeRefsTowers->chanAttrListPair(emCoolId);
	    if (emItr != m_dbFineTimeRefsTowers->end()) {
	       const AthenaAttributeList& emAttrList(emItr->second); 
	       emReference = emAttrList["referenceValue"].data<double>();
	       emCalFactor = emAttrList["calibrationFactor"].data<double>();
	    }
	    m_fineTimePlotManager->SetEmReferenceValue(emReference);
	    m_fineTimePlotManager->SetEmCalibrationFactor(emCalFactor);
	    
	    CondAttrListCollection::const_iterator hadItr = m_dbFineTimeRefsTowers->chanAttrListPair(hadCoolId);
	    if (hadItr != m_dbFineTimeRefsTowers->end()) {
	       const AthenaAttributeList& hadAttrList(hadItr->second); 
	       hadReference = hadAttrList["referenceValue"].data<double>();
	       hadCalFactor = hadAttrList["calibrationFactor"].data<double>();
	    }
	    m_fineTimePlotManager->SetHadReferenceValue(hadReference);
	    m_fineTimePlotManager->SetHadCalibrationFactor(hadCalFactor);
	        
	    m_fineTimePlotManager->Analyze(m_eventInfo,*TT_itr, EmIsDisabled, HadIsDisabled);
	}
	if (m_doPedestalPlots)
	{
	    m_pedestalPlotManager->Analyze(m_eventInfo,*TT_itr, EmIsDisabled, HadIsDisabled);
	}
	if (m_doEtCorrelationPlots)
	{
	    m_etCorrelationPlotManager->Analyze(m_eventInfo,*TT_itr, EmIsDisabled, HadIsDisabled);
	}
    }
    
    return sc;
}

StatusCode L1CaloPprMonitoring::finalize()
{

    if (m_doFineTimePlots)
    {
        m_fineTimePlotManager->MakeSummary();
	delete m_fineTimePlotManager;
    }
    if (m_doPedestalPlots)
    {
        m_pedestalPlotManager->MakeSummary();
	delete m_pedestalPlotManager;
    }
    if (m_doEtCorrelationPlots)
    {
        m_etCorrelationPlotManager->MakeSummary();
	delete m_etCorrelationPlotManager;
    }    
    return StatusCode::SUCCESS;
}

StatusCode L1CaloPprMonitoring::loadContainers()
{
    StatusCode sc;

    sc = m_storeGate->retrieve(m_eventInfo);
    if(sc.isFailure()){msg(MSG::INFO)<<"Failed to load Event Info"<<endreq; return sc;}

    sc = m_storeGate->retrieve(m_dbPpmDeadChannels, m_dbPpmDeadChannelsFolder);
    if(sc.isFailure()){msg(MSG::INFO)<<"Failed to load DB PPm Dead channels"<<endreq; return sc;}

    sc = m_storeGate->retrieve(m_dbPpmDisabledTowers,m_dbPpmDisabledTowersFolder);
    if(sc.isFailure()){msg(MSG::INFO)<<"Failed to load DB PPm Disabled Towers"<<endreq; return sc;}

    sc = m_storeGate->retrieve(m_triggerTowers,m_triggerTowersLocation);
    if(sc.isFailure()){msg(MSG::INFO)<<"Failed to load Trigger Towers Location"<<endreq; return sc;}
    
    if (m_doEtCorrelationPlots) {
        StatusCode sc2 = m_etCorrelationPlotManager->getCaloCells();
    }
    
    if (m_doFineTimePlots){
      //load the standalone sqlite db containing the fine time references and calibration values
      sc = m_storeGate->retrieve(m_dbFineTimeRefsTowers, m_dbFineTimeRefsFolder);
      if (sc.isFailure()) {msg(MSG::WARNING) << "Failed to load FineTimeReference Folder"<< endreq; return sc;}
      //if you want to use the calo quality cut also load the calo cell container
      if(m_doCaloQualCut){
	StatusCode sc2 = m_fineTimePlotManager->getCaloCells();
	if (sc2.isFailure()) {msg(MSG::WARNING) << "Failed to load Calo Cells"<< endreq; return sc2;}
      }
    } 
    return sc;
}

StatusCode L1CaloPprMonitoring::loadInTools()
{
    StatusCode sc;
    
    sc = service("THistSvc",m_histoSvc);
    if(sc.isFailure()){msg(MSG::INFO)<<"Failed to load the histogram service"<<endreq; return sc;}

    sc = m_towerTools.retrieve();
    if(sc.isFailure()){msg(MSG::ERROR)<<"Could not retrieve TriggerTowerTools"<<endreq;return sc;}
       
    return sc;
}
