/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloPprMonitoring.h"

#include "TH1.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ITHistSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include "TrigT1CaloCalibTools/L1CaloPprFineTimePlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprPedestalPlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprPedestalCorrectionPlotManager.h"
#include "TrigT1CaloCalibTools/L1CaloPprEtCorrelationPlotManager.h"

L1CaloPprMonitoring::L1CaloPprMonitoring(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator),
    m_eventInfo(nullptr),
    m_dbPpmDeadChannels(nullptr),
    m_dbPpmDisabledTowers(nullptr),
    m_dbFineTimeRefsTowers(nullptr),
    m_triggerTowers(nullptr),
    m_histoSvc("THistSvc", name),
    m_fineTimePlotManager(0),
    m_pedestalPlotManager(0),
    m_pedestalCorrectionPlotManager(0),
    m_etCorrelationPlotManager(0),
    m_offlineTowerTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_towerTools("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_dbPpmDeadChannelsFolder("/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"),
    m_dbPpmDisabledTowersFolder("/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"),
    m_dbFineTimeRefsFolder("/TRIGGER/L1Calo/V1/References/FineTimeReferences"),
    m_PpmAdcMinValue(0),
    m_lumiBlockMax(0),
    m_doFineTimePlots(0),
    m_doPedestalPlots(0),
    m_doPedestalCorrectionPlots(0),
    m_doEtCorrelationPlots(0),
    m_fineTimeCut(0),
    m_pedestalMaxWidth(0),
    m_caloCellContainerName(""),
    m_EtMinForEtCorrelation(0)									      
{
    declareProperty("TriggerTowersLocation",m_triggerTowersLocation=LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
    declareProperty("ppmADCMinValue",m_PpmAdcMinValue=60);
    declareProperty("ppmADCMaxValue",m_PpmAdcMaxValue=1023); 
    declareProperty("lumiBlockMax",m_lumiBlockMax=2000);
    declareProperty("doFineTimePlots",m_doFineTimePlots=true);
    declareProperty("doPedestalPlots",m_doPedestalPlots=true);
    declareProperty("doPedestalCorrectionPlots",m_doPedestalCorrectionPlots=true);
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
    CHECK(this->loadInTools());

    if (m_doFineTimePlots)
    {
        m_fineTimePlotManager = new L1CaloPprFineTimePlotManager(&(*m_histoSvc),
								 m_offlineTowerTools,
								 m_PpmAdcMinValue,
								 m_lumiBlockMax,
								 "L1Calo/PPM");
	m_fineTimePlotManager->SetFineTimeCut(m_fineTimeCut);
	m_fineTimePlotManager->SetPpmAdcMaxValue(m_PpmAdcMaxValue); 
	m_fineTimePlotManager->EnableCaloQualCut(m_doCaloQualCut);
    }
    if (m_doPedestalPlots)
    {
        m_pedestalPlotManager = new L1CaloPprPedestalPlotManager(&(*m_histoSvc),
								 m_offlineTowerTools,
								 m_lumiBlockMax,
								 "L1Calo/PPM");
	m_pedestalPlotManager->SetPedestalMaxWidth(m_pedestalMaxWidth);
    }
    if (m_doPedestalCorrectionPlots)
    {
        m_pedestalCorrectionPlotManager = new L1CaloPprPedestalCorrectionPlotManager(&(*m_histoSvc),
								 m_offlineTowerTools,
								 m_lumiBlockMax,
								 "L1Calo/PPM");
    }
    if (m_doEtCorrelationPlots){
        m_etCorrelationPlotManager = new L1CaloPprEtCorrelationPlotManager(&(*m_histoSvc),
									   m_offlineTowerTools,
									   m_lumiBlockMax,
									   "L1Calo/PPM");
	m_etCorrelationPlotManager->SetCaloCellContainer(m_caloCellContainerName);
	m_etCorrelationPlotManager->SetEtMin(m_EtMinForEtCorrelation);
    }
    return StatusCode::SUCCESS;
}

StatusCode L1CaloPprMonitoring::execute()
{
    CHECK(this->loadContainers());
    for(const auto* tt: *m_triggerTowers) {
        //Dead Channel DB folder used for 2010 data
        //Disabled tower DB folder used for 2011/12 data
        
// 	const coral::AttributeList* DbDead = 0;

	bool ChanIsDisabled(false);
	unsigned int CoolChannelID = tt->coolId();
//         if(m_eventInfo->event_ID()->run_number() < 175000)
//         {
// 	    DbDead      = m_xAODtowerTools->DbAttributes(tt, m_dbPpmDeadChannels);
// 
//             if(DbDead!=0){ChanIsDisabled = true;}
//         }
//         else //2011/12 data 
//         {
// 	    DbDead      = m_xAODtowerTools->DbAttributes(tt, m_dbPpmDisabledTowers);
//             
// 	    if(m_towerTools->DisabledTower(DbDead)){ChanIsDisabled= true; }
//         }
        
        ChanIsDisabled = m_towerTools->disabledChannel(CoolChannelID);
	if (m_doFineTimePlots)
	{
	    //Set the reference and calibration values for the fine time, they are stored per cool ID in a data base
	    double Reference = 0;
	    double CalFactor = 0;
	  
	    unsigned int CoolId = tt->coolId();

	    CondAttrListCollection::const_iterator Itr = m_dbFineTimeRefsTowers->chanAttrListPair(CoolId);
	    if (Itr != m_dbFineTimeRefsTowers->end()) {
               const coral::AttributeList& AttrList =Itr->second;
	       Reference = AttrList["referenceValue"].data<double>();
	       CalFactor = AttrList["calibrationFactor"].data<double>();
	    }
	    
	    m_fineTimePlotManager->SetReferenceValue(Reference);
	    m_fineTimePlotManager->SetCalibrationFactor(CalFactor);
	        
	    m_fineTimePlotManager->Analyze(m_eventInfo, tt, ChanIsDisabled);
	}
	if (m_doPedestalPlots)
	{
	    m_pedestalPlotManager->Analyze(m_eventInfo, tt, ChanIsDisabled);
	}
	if (m_doPedestalCorrectionPlots)
	{
	    m_pedestalCorrectionPlotManager->Analyze(m_eventInfo, tt, ChanIsDisabled);	    
	}
	if (m_doEtCorrelationPlots)
	{
	    m_etCorrelationPlotManager->Analyze(m_eventInfo, tt, ChanIsDisabled);
	}
    }
    return StatusCode::SUCCESS;
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
    if (m_doPedestalCorrectionPlots)
    {
      m_pedestalCorrectionPlotManager->MakeSummary();
      delete m_pedestalCorrectionPlotManager;
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
    CHECK(evtStore()->retrieve(m_eventInfo));
    CHECK(detStore()->retrieve(m_dbPpmDeadChannels, m_dbPpmDeadChannelsFolder));
    CHECK(detStore()->retrieve(m_dbPpmDisabledTowers,m_dbPpmDisabledTowersFolder));
    CHECK(evtStore()->retrieve(m_triggerTowers,m_triggerTowersLocation));
    
    if (m_doEtCorrelationPlots) {
      CHECK(m_etCorrelationPlotManager->getCaloCells());
    }
    
    if (m_doFineTimePlots){
      //load the standalone sqlite db containing the fine time references and calibration values
      CHECK(detStore()->retrieve(m_dbFineTimeRefsTowers, m_dbFineTimeRefsFolder));

      //if you want to use the calo quality cut also load the calo cell container
      if(m_doCaloQualCut){
	CHECK(m_fineTimePlotManager->getCaloCells());
      }
    } 
    return StatusCode::SUCCESS;
}

StatusCode L1CaloPprMonitoring::loadInTools()
{
    CHECK(m_histoSvc.retrieve());
    CHECK(m_offlineTowerTools.retrieve());
    CHECK(m_towerTools.retrieve());
    return StatusCode::SUCCESS;
}
