/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JEPSimMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <utility>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloToolInterfaces/IL1JetCMXTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"
#include "TrigT1CaloToolInterfaces/IL1EnergyCMXTools.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "JEPSimMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
JEPSimMon::JEPSimMon(const std::string & type, 
	 	     const std::string & name,
		     const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_jetCmxTool("LVL1::L1JetCMXTools/L1JetCMXTools"),
    m_jetTool("LVL1::L1JetTools/L1JetTools"),
    m_jetElementTool("LVL1::L1JetElementTools/L1JetElementTools"),
    m_energyCmxTool("LVL1::L1EnergyCMXTools/L1EnergyCMXTools"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_debug(false), m_rodTES(0), m_limitedRoi(0),
    m_histBooked(false),
    m_h_jem_em_2d_etaPhi_jetEl_SimEqCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNeCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNoCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimEqCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNeCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNoCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim(0),
    m_h_jem_2d_roi_EnergyLgSimEqData(0),
    m_h_jem_2d_roi_EnergyLgSimNeData(0),
    m_h_jem_2d_roi_EnergyLgSimNoData(0),
    m_h_jem_2d_roi_EnergyLgDataNoSim(0),
    m_h_jem_2d_roi_EnergySmSimEqData(0),
    m_h_jem_2d_roi_EnergySmSimNeData(0),
    m_h_jem_2d_roi_EnergySmSimNoData(0),
    m_h_jem_2d_roi_EnergySmDataNoSim(0),
    m_h_jem_2d_etaPhi_roi_SimEqData(0),
    m_h_jem_2d_etaPhi_roi_SimNeData(0),
    m_h_jem_2d_etaPhi_roi_SimNoData(0),
    m_h_jem_2d_etaPhi_roi_DataNoSim(0),
    m_h_cmx_2d_tob_EnergyLgSimEqData(0),
    m_h_cmx_2d_tob_EnergyLgSimNeData(0),
    m_h_cmx_2d_tob_EnergyLgSimNoData(0),
    m_h_cmx_2d_tob_EnergyLgDataNoSim(0),
    m_h_cmx_2d_tob_EnergySmSimEqData(0),
    m_h_cmx_2d_tob_EnergySmSimNeData(0),
    m_h_cmx_2d_tob_EnergySmSimNoData(0),
    m_h_cmx_2d_tob_EnergySmDataNoSim(0),
    m_h_cmx_2d_etaPhi_tob_SimEqData(0),
    m_h_cmx_2d_etaPhi_tob_SimNeData(0),
    m_h_cmx_2d_etaPhi_tob_SimNoData(0),
    m_h_cmx_2d_etaPhi_tob_DataNoSim(0),
    m_h_cmx_1d_thresh_SumsSimEqData(0),
    m_h_cmx_1d_thresh_SumsSimNeData(0),
    m_h_cmx_1d_thresh_SumsSimNoData(0),
    m_h_cmx_1d_thresh_SumsDataNoSim(0),
    m_h_cmx_1d_thresh_SumsOvfSimEqData(0),
    m_h_cmx_1d_thresh_SumsOvfSimNeData(0),
    m_h_cmx_2d_thresh_SumsThreshSimEqData(0),
    m_h_cmx_2d_thresh_SumsThreshSimNeData(0),
    m_h_cmx_2d_topo_SimEqData(0),
    m_h_cmx_2d_topo_SimNeData(0),
    m_h_cmx_2d_topo_SimNoData(0),
    m_h_cmx_2d_topo_DataNoSim(0),
    m_h_jem_2d_energy_SimEqData(0),
    m_h_jem_2d_energy_SimNeData(0),
    m_h_jem_2d_energy_SimNoData(0),
    m_h_jem_2d_energy_DataNoSim(0),
    m_h_cmx_2d_energy_JemEqCmx(0),
    m_h_cmx_2d_energy_JemNeCmx(0),
    m_h_cmx_2d_energy_JemNoCmx(0),
    m_h_cmx_2d_energy_CmxNoJem(0),
    m_h_cmx_2d_energy_SumsSimEqData(0),
    m_h_cmx_2d_energy_SumsSimNeData(0),
    m_h_cmx_2d_energy_SumsSimNoData(0),
    m_h_cmx_2d_energy_SumsDataNoSim(0),
    m_h_cmx_2d_energy_EtMapsThreshSimEqData(0),
    m_h_cmx_2d_energy_EtMapsThreshSimNeData(0),
    m_h_jem_2d_SimEqDataOverview(0),
    m_h_jem_2d_SimNeDataOverview(0),
    m_h_jem_1d_SimNeDataSummary(0),
    m_v_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  declareProperty("JetCMXTool", m_jetCmxTool);
  declareProperty("JetTool", m_jetTool);
  declareProperty("JetElementTool", m_jetElementTool);
  declareProperty("EnergyCMXTool", m_energyCmxTool);
  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);

  declareProperty("JetElementLocation",
                 m_jetElementLocation =
		                 LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JetElementLocationOverlap",
                 m_jetElementLocationOverlap =
		           LVL1::TrigT1CaloDefs::JetElementLocation+"Overlap");
  declareProperty("CMXJetTobLocation",
                 m_cmxJetTobLocation  = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty("CMXJetHitsLocation",
                 m_cmxJetHitsLocation = LVL1::TrigT1CaloDefs::CMXJetHitsLocation);
  declareProperty("JEMTobRoILocation",
                 m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMTobRoILocation);
  declareProperty("CMXRoILocation",
                 m_cmxRoiLocation     = LVL1::TrigT1CaloDefs::CMXRoILocation);
  declareProperty("JEMEtSumsLocation",
                 m_jemEtSumsLocation  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMXEtSumsLocation",
                 m_cmxEtSumsLocation  = LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		                 LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("RodHeaderLocation",
                 m_rodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation);
  declareProperty("ErrorLocation",
                 m_errorLocation     = "L1CaloJEMMismatchVector");

  declareProperty("RootDirectory", m_rootDir = "L1Calo/JEM");
}

/*---------------------------------------------------------*/
JEPSimMon::~JEPSimMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEPSimMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_jetElementTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JetElementTools"
                    << endreq;
    return sc;
  }

  sc = m_jetTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JetTools" << endreq;
    return sc;
  }

  sc = m_jetCmxTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JetCMXTools" << endreq;
    return sc;
  }

  sc = m_energyCmxTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1EnergyCMXTools" << endreq;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newEventsBlock || newLumiBlock ) { }

  if( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
  std::string dir1(m_rootDir + "/Errors/Transmission_Simulation");
  MonGroup monShift( this, dir1, run, attr );
  MonGroup monExpert( this, dir1, run, attr );
  MonGroup monElements( this, dir1 + "/PPM2Elements", run, attr );
  MonGroup monRoIs( this, dir1 + "/Elements2RoIs", run, attr );
  MonGroup monEnergy( this, dir1 + "/Elements2Energy", run, attr );
  MonGroup monEvent1( this, dir1 + "/MismatchEventNumbers", run, attr, "", "eventSample" );
  std::string dir2(m_rootDir + "_CMX/Errors/Transmission_Simulation");
  MonGroup monTobs( this, dir2 + "/RoIs2TOBs", run, attr );
  MonGroup monHitSums( this, dir2 + "/TOBs2HitSums", run, attr );
  MonGroup monTopo( this, dir2 + "/TOBs2Topo", run, attr );
  MonGroup monEnergy2( this, dir2 + "/JEM2CMXEnergy", run, attr );
  MonGroup monEnergySums( this, dir2 + "/Energy2Sums", run, attr );
  MonGroup monEvent2( this, dir2 + "/MismatchEventNumbers", run, attr, "", "eventSample" );

  // JetElements

  m_histTool->setMonGroup(&monElements);

  m_h_jem_em_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimEqCore",
    "Core Jet Elements EM Data/Simulation Non-zero Matches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNeCore",
    "Core Jet Elements EM Data/Simulation Non-zero Mismatches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNoCore",
    "Core Jet Elements EM Simulation but no Data");
  m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_CoreNoSim",
    "Core Jet Elements EM Data but no Simulation");
  m_h_jem_had_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimEqCore",
    "Core Jet Elements HAD Data/Simulation Non-zero Matches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNeCore",
    "Core Jet Elements HAD Data/Simulation Non-zero Mismatches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNoCore",
    "Core Jet Elements HAD Simulation but no Data");
  m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_CoreNoSim",
    "Core Jet Elements HAD Data but no Simulation");
  m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimEqOverlap",
    "Overlap Jet Elements EM Data/Simulation Non-zero Matches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNeOverlap",
    "Overlap Jet Elements EM Data/Simulation Non-zero Mismatches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNoOverlap",
    "Overlap Jet Elements EM Simulation but no Data");
  m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_OverlapNoSim",
    "Overlap Jet Elements EM Data but no Simulation");
  m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimEqOverlap",
    "Overlap Jet Elements HAD Data/Simulation Non-zero Matches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNeOverlap",
    "Overlap Jet Elements HAD Data/Simulation Non-zero Mismatches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNoOverlap",
    "Overlap Jet Elements HAD Simulation but no Data");
  m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_OverlapNoSim",
    "Overlap Jet Elements HAD Data but no Simulation");

  //  RoIs

  m_histTool->setMonGroup(&monRoIs);

  m_h_jem_2d_roi_EnergyLgSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergyLgSimEqData", "JEM RoI Energy Large Data/Simulation Non-zero Matches");
  m_h_jem_2d_roi_EnergyLgSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergyLgSimNeData", "JEM RoI Energy Large Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_roi_EnergyLgSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergyLgSimNoData", "JEM RoI Energy Large Simulation but no Data");
  m_h_jem_2d_roi_EnergyLgDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergyLgDataNoSim", "JEM RoI Energy Large Data but no Simulation");
  m_h_jem_2d_roi_EnergySmSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergySmSimEqData", "JEM RoI Energy Small Data/Simulation Non-zero Matches");
  m_h_jem_2d_roi_EnergySmSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergySmSimNeData", "JEM RoI Energy Small Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_roi_EnergySmSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergySmSimNoData", "JEM RoI Energy Small Simulation but no Data");
  m_h_jem_2d_roi_EnergySmDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_EnergySmDataNoSim", "JEM RoI Energy Small Data but no Simulation");

  m_h_jem_2d_etaPhi_roi_SimEqData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimEqData", "JEM RoI Data/Simulation Non-zero Matches");
  m_h_jem_2d_etaPhi_roi_SimNeData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimNeData", "JEM RoI Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_etaPhi_roi_SimNoData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimNoData", "JEM RoI Simulation but no Data");
  m_h_jem_2d_etaPhi_roi_DataNoSim = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_DataNoSim", "JEM RoI Data but no Simulation");

  //  CMX-Jet TOBs

  m_histTool->setMonGroup(&monTobs);

  m_h_cmx_2d_tob_EnergyLgSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergyLgSimEqData", "CMX TOB Energy Large Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_EnergyLgSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergyLgSimNeData", "CMX TOB Energy Large Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_EnergyLgSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergyLgSimNoData", "CMX TOB Energy Large Simulation but no Data");
  m_h_cmx_2d_tob_EnergyLgDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergyLgDataNoSim", "CMX TOB Energy Large Data but no Simulation");
  m_h_cmx_2d_tob_EnergySmSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergySmSimEqData", "CMX TOB Energy Small Data/Simulation Non-zero Matches");
  m_h_cmx_2d_tob_EnergySmSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergySmSimNeData", "CMX TOB Energy Small Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_tob_EnergySmSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergySmSimNoData", "CMX TOB Energy Small Simulation but no Data");
  m_h_cmx_2d_tob_EnergySmDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "cmx_2d_tob_EnergySmDataNoSim", "CMX TOB Energy Small Data but no Simulation");

  m_h_cmx_2d_etaPhi_tob_SimEqData = m_histTool->bookJEMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimEqData", "CMX TOB Data/Simulation Non-zero Matches");
  m_h_cmx_2d_etaPhi_tob_SimNeData = m_histTool->bookJEMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimNeData", "CMX TOB Data/Simulation Non-zero Mismatches");
  m_h_cmx_2d_etaPhi_tob_SimNoData = m_histTool->bookJEMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_SimNoData", "CMX TOB Simulation but no Data");
  m_h_cmx_2d_etaPhi_tob_DataNoSim = m_histTool->bookJEMRoIEtaVsPhi(
    "cmx_2d_etaPhi_tob_DataNoSim", "CMX TOB Data but no Simulation");

  m_histTool->setMonGroup(&monHitSums);

  // Local/Remote/Total sums

  m_h_cmx_1d_thresh_SumsSimEqData = m_histTool->book1F(
    "cmx_1d_thresh_SumsSimEqData",
    "CMX Hit Sums Data/Simulation Non-zero Matches", 4, 0, 4);
  setLabelsSH(m_h_cmx_1d_thresh_SumsSimEqData);
  m_h_cmx_1d_thresh_SumsSimNeData = m_histTool->book1F(
    "cmx_1d_thresh_SumsSimNeData",
    "CMX Hit Sums Data/Simulation Non-zero Mismatches", 4, 0, 4);
  setLabelsSH(m_h_cmx_1d_thresh_SumsSimNeData);
  m_h_cmx_1d_thresh_SumsSimNoData = m_histTool->book1F(
    "cmx_1d_thresh_SumsSimNoData",
    "CMX Hit Sums Simulation but no Data", 4, 0, 4);
  setLabelsSH(m_h_cmx_1d_thresh_SumsSimNoData);
  m_h_cmx_1d_thresh_SumsDataNoSim = m_histTool->book1F(
    "cmx_1d_thresh_SumsDataNoSim",
    "CMX Hit Sums Data but no Simulation", 4, 0, 4);
  setLabelsSH(m_h_cmx_1d_thresh_SumsDataNoSim);
  m_h_cmx_1d_thresh_SumsOvfSimEqData = m_histTool->book1F(
    "cmx_1d_thresh_SumsOvfSimEqData",
    "CMX Hit Sums RoI Overflow Bit Data/Simulation Matches", 2, 0, 2);
  LWHist::LWHistAxis* axis = m_h_cmx_1d_thresh_SumsOvfSimEqData->GetXaxis();
  axis->SetBinLabel(1, "Remote");
  axis->SetBinLabel(2, "Total");
  m_h_cmx_1d_thresh_SumsOvfSimNeData = m_histTool->book1F(
    "cmx_1d_thresh_SumsOvfSimNeData",
    "CMX Hit Sums RoI Overflow Bit Data/Simulation Mismatches", 2, 0, 2);
  axis = m_h_cmx_1d_thresh_SumsOvfSimNeData->GetXaxis();
  axis->SetBinLabel(1, "Remote");
  axis->SetBinLabel(2, "Total");
  m_h_cmx_2d_thresh_SumsThreshSimEqData = m_histTool->book2F(
    "cmx_2d_thresh_SumsThreshSimEqData",
    "CMX Hit Sums Data/Simulation Threshold Matches", 4, 0, 4, 25, 0, 25);
  setLabelsSHF(m_h_cmx_2d_thresh_SumsThreshSimEqData);
  m_h_cmx_2d_thresh_SumsThreshSimNeData = m_histTool->book2F(
    "cmx_2d_thresh_SumsThreshSimNeData",
    "CMX Hit Sums Data/Simulation Threshold Mismatches", 4, 0, 4, 25, 0, 25);
  setLabelsSHF(m_h_cmx_2d_thresh_SumsThreshSimNeData);

  // Topo output information

  m_histTool->setMonGroup(&monTopo);

  m_h_cmx_2d_topo_SimEqData = m_histTool->book2F(
    "cmx_2d_topo_SimEqData", "CMX Topo Output Data/Simulation Non-zero Matches",
    2, 0, 2, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimEqData);
  m_h_cmx_2d_topo_SimNeData = m_histTool->book2F(
    "cmx_2d_topo_SimNeData", "CMX Topo Output Data/Simulation Non-zero Mismatches",
    2, 0, 2, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimNeData);
  m_h_cmx_2d_topo_SimNoData = m_histTool->book2F(
    "cmx_2d_topo_SimNoData", "CMX Topo Output Simulation but no Data",
    2, 0, 2, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_SimNoData);
  m_h_cmx_2d_topo_DataNoSim = m_histTool->book2F(
    "cmx_2d_topo_DataNoSim", "CMX Topo Output Data but no Simulation",
    2, 0, 2, 3, 0, 3);
  setLabelsTopo(m_h_cmx_2d_topo_DataNoSim);

  // JEMEtSums

  m_histTool->setMonGroup(&monEnergy);

  m_h_jem_2d_energy_SimEqData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimEqData", "JEM EtSums Data/Simulation Non-zero Matches");
  m_h_jem_2d_energy_SimNeData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimNeData",
    "JEM EtSums Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_energy_SimNoData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimNoData", "JEM EtSums Simulation but no Data");
  m_h_jem_2d_energy_DataNoSim = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_DataNoSim", "JEM EtSums Data but no Simulation");

  // CMXEtSums

  m_histTool->setMonGroup(&monEnergy2);

  m_h_cmx_2d_energy_JemEqCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmx_2d_energy_JemEqCmx", "CMX EtSums/JEM EtSums Non-zero Matches");
  m_h_cmx_2d_energy_JemNeCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmx_2d_energy_JemNeCmx", "CMX EtSums/JEM EtSums Non-zero Mismatches");
  m_h_cmx_2d_energy_JemNoCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmx_2d_energy_JemNoCmx", "JEM EtSums but no CMX EtSums");
  m_h_cmx_2d_energy_CmxNoJem = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmx_2d_energy_CmxNoJem", "CMX EtSums but no JEM EtSums");

  m_histTool->setMonGroup(&monEnergySums);

  // Energy Crate/System sums

  m_h_cmx_2d_energy_SumsSimEqData = m_histTool->book2F(
    "cmx_2d_energy_SumsSimEqData",
    "Energy Totals Data/Simulation Non-zero Matches", 5, 0, 5, 11, 0, 11);
  setLabelsEnTot(m_h_cmx_2d_energy_SumsSimEqData);
  m_h_cmx_2d_energy_SumsSimNeData = m_histTool->book2F(
    "cmx_2d_energy_SumsSimNeData",
    "Energy Totals Data/Simulation Non-zero Mismatches", 5, 0, 5, 11, 0, 11);
  setLabelsEnTot(m_h_cmx_2d_energy_SumsSimNeData);
  m_h_cmx_2d_energy_SumsSimNoData = m_histTool->book2F(
    "cmx_2d_energy_SumsSimNoData",
    "Energy Totals Simulation but no Data", 5, 0, 5, 11, 0, 11);
  setLabelsEnTot(m_h_cmx_2d_energy_SumsSimNoData);
  m_h_cmx_2d_energy_SumsDataNoSim = m_histTool->book2F(
    "cmx_2d_energy_SumsDataNoSim",
    "Energy Totals Data but no Simulation", 5, 0, 5, 11, 0, 11);
  setLabelsEnTot(m_h_cmx_2d_energy_SumsDataNoSim);
  m_h_cmx_2d_energy_EtMapsThreshSimEqData = m_histTool->book2F(
    "cmx_2d_energy_EtMapsThreshSimEqData",
    "Et Maps Data/Simulation Threshold Matches", 10, 0, 10, 24, 0, 24);
  setLabelsEnTotThr(m_h_cmx_2d_energy_EtMapsThreshSimEqData);
  m_h_cmx_2d_energy_EtMapsThreshSimNeData = m_histTool->book2F(
    "cmx_2d_energy_EtMapsThreshSimNeData",
    "Et Maps Data/Simulation Threshold Mismatches", 10, 0, 10, 24, 0, 24);
  setLabelsEnTotThr(m_h_cmx_2d_energy_EtMapsThreshSimNeData);

  // Summary

  m_histTool->setMonGroup(&monExpert);

  m_h_jem_2d_SimEqDataOverview = m_histTool->book2F("jem_2d_SimEqDataOverview",
   "JEP Transmission/Comparison with Simulation Overview - Events with Matches;Crate/Module",
             36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->jemCMXCrateModule(m_h_jem_2d_SimEqDataOverview);
  setLabels(m_h_jem_2d_SimEqDataOverview, false);

  m_h_jem_2d_SimNeDataOverview = m_histTool->book2F("jem_2d_SimNeDataOverview",
"JEP Transmission/Comparison with Simulation Overview - Events with Mismatches;Crate/Module",
             36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->jemCMXCrateModule(m_h_jem_2d_SimNeDataOverview);
  setLabels(m_h_jem_2d_SimNeDataOverview, false);

  m_histTool->setMonGroup(&monShift);

  m_h_jem_1d_SimNeDataSummary = m_histTool->book1F("jem_1d_SimNeDataSummary",
   "JEP Transmission/Comparison with Simulation Mismatch Summary;;Events",
    NumberOfSummaryBins, 0, NumberOfSummaryBins);
  setLabels(m_h_jem_1d_SimNeDataSummary);

  // Mismatch Event Number Samples

  m_histTool->setMonGroup(&monEvent1);

  TH2I_LW* hist = 0;
  m_v_2d_MismatchEvents.clear();
  m_v_2d_MismatchEvents.resize(8, hist);
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_em_2d_jetEl_MismatchEvents",
    "Jet Elements EM Mismatch Event Numbers");
  m_v_2d_MismatchEvents[0] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_had_2d_jetEl_MismatchEvents",
    "Jet Elements Had Mismatch Event Numbers");
  m_v_2d_MismatchEvents[1] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_2d_roi_MismatchEvents", "JEM RoIs Mismatch Event Numbers");
  m_v_2d_MismatchEvents[2] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_2d_energy_MismatchEvents", "JEM Energy Mismatch Event Numbers");
  m_v_2d_MismatchEvents[4] = hist;

  m_histTool->setMonGroup(&monEvent2);

  hist = m_histTool->bookJEMEventVsCrateModule(
    "cmx_2d_tob_MismatchEvents", "CMX TOBs Mismatch Event Numbers");
  m_v_2d_MismatchEvents[3] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "cmx_2d_energy_MismatchEvents", "CMX Energy Mismatch Event Numbers");
  m_v_2d_MismatchEvents[5] = hist;
  hist = m_histTool->bookEventNumbers("cmx_2d_thresh_SumsMismatchEvents",
    "CMX Hit Sums Mismatch Event Numbers", 6, 0., 6.);
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Local 0");
  axis->SetBinLabel(2, "Local 1");
  axis->SetBinLabel(3, "Remote");
  axis->SetBinLabel(4, "Total");
  axis->SetBinLabel(5, "Topo 0");
  axis->SetBinLabel(6, "Topo 1");
  m_v_2d_MismatchEvents[6] = hist;
  hist = m_histTool->bookEventNumbers("cmx_2d_energy_SumsMismatchEvents",
    "CMX Energy Sums Mismatch Event Numbers", 10, 0., 10.);
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Modules 0");
  axis->SetBinLabel(2, "Modules 1");
  axis->SetBinLabel(3, "Local 0");
  axis->SetBinLabel(4, "Local 1");
  axis->SetBinLabel(5, "Remote");
  axis->SetBinLabel(6, "Total");
  axis->SetBinLabel(7, "SumEt");
  axis->SetBinLabel(8, "MissingEt");
  axis->SetBinLabel(9, "MissEtSig");
  axis->SetBinLabel(10, "Engy RoIs");
  m_v_2d_MismatchEvents[7] = hist;

  m_histTool->unsetMonGroup();
  m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endreq;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "fillHistograms entered" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endreq; 
  }

  //Retrieve Core and Overlap Jet Elements from SG
  const JetElementCollection* jetElementTES = 0; 
  const JetElementCollection* jetElementOvTES = 0; 
  sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation); 
  if( sc.isFailure()  ||  !jetElementTES ) {
    msg(MSG::DEBUG) << "No Core Jet Element container found" << endreq; 
  }
  sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
  if( sc.isFailure()  ||  !jetElementOvTES ) {
    msg(MSG::DEBUG) << "No Overlap Jet Element container found" << endreq;
  }
  
  //Retrieve JEM RoIs from SG
  const JemRoiCollection* jemRoiTES = 0;
  sc = evtStore()->retrieve( jemRoiTES, m_jemRoiLocation);
  if( sc.isFailure()  ||  !jemRoiTES  ||  jemRoiTES->empty() ) {
    msg(MSG::DEBUG) << "No DAQ JEM RoIs container found" << endreq; 
  }

  //Retrieve ROD Headers from SG
  m_rodTES = 0;
  m_limitedRoi = 0;
  sc = evtStore()->retrieve( m_rodTES, m_rodHeaderLocation);
  if( sc.isFailure()  ||  !m_rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endreq;
  }
  
  //Retrieve CMX-Jet TOBs from SG
  const CmxJetTobCollection* cmxJetTobTES = 0;
  sc = evtStore()->retrieve( cmxJetTobTES, m_cmxJetTobLocation);
  if( sc.isFailure()  ||  !cmxJetTobTES ) {
    msg(MSG::DEBUG) << "No CMX-Jet TOB container found" << endreq; 
  }
  
  //Retrieve CMX-Jet Hits from SG
  const CmxJetHitsCollection* cmxJetHitsTES = 0;
  sc = evtStore()->retrieve( cmxJetHitsTES, m_cmxJetHitsLocation);
  if( sc.isFailure()  ||  !cmxJetHitsTES ) {
    msg(MSG::DEBUG) << "No CMX-Jet Hits container found" << endreq; 
  }
  
  //Retrieve CMX RoIs from SG
  const LVL1::CMXRoI* cmxRoiTES = 0;
  sc = evtStore()->retrieve( cmxRoiTES, m_cmxRoiLocation);
  if( sc.isFailure()  ||  !cmxRoiTES ) {
    msg(MSG::DEBUG) << "No DAQ CMX RoIs found" << endreq; 
  }

  //Retrieve JEM Et Sums from SG
  const JemEtSumsCollection* jemEtSumsTES = 0;
  sc = evtStore()->retrieve( jemEtSumsTES, m_jemEtSumsLocation);
  if( sc.isFailure()  ||  !jemEtSumsTES ) {
    msg(MSG::DEBUG) << "No JEM Et Sums container found" << endreq;
  }

  //Retrieve CMX Et Sums from SG
  const CmxEtSumsCollection* cmxEtSumsTES = 0;
  sc = evtStore()->retrieve( cmxEtSumsTES, m_cmxEtSumsLocation);
  if( sc.isFailure()  ||  !cmxEtSumsTES ) {
    msg(MSG::DEBUG) << "No CMX-Energy Et Sums container found" << endreq;
  }

  // Maps to simplify comparisons
  
  JetElementMap jeMap;
  JetElementMap ovMap;
  JemRoiMap     jrMap;
  CmxJetTobMap  ctMap;
  CmxJetHitsMap chMap;
  JemEtSumsMap  jsMap;
  CmxEtSumsMap  csMap;
  setupMap(jetElementTES,   jeMap);
  setupMap(jetElementOvTES, ovMap);
  setupMap(jemRoiTES,       jrMap);
  setupMap(cmxJetTobTES,    ctMap);
  setupMap(cmxJetHitsTES,   chMap);
  setupMap(jemEtSumsTES,    jsMap);
  setupMap(cmxEtSumsTES,    csMap);

  // Vectors for error overview bits;
  const int vecsizeJem = 2 * s_crates * s_modules;
  const int vecsizeCmx = 2 * s_crates * s_cmxs;
  ErrorVector errorsJEM(vecsizeJem);
  ErrorVector errorsCMX(vecsizeCmx);

  // Compare Jet Elements simulated from Trigger Towers with Jet Elements
  // from data

  JetElementCollection* jetElementSIM = 0;
  if (triggerTowerTES) {
    jetElementSIM = new JetElementCollection;
    simulate(triggerTowerTES, jetElementSIM);
  }
  JetElementMap jeSimMap;
  setupMap(jetElementSIM, jeSimMap);
  bool overlap = false;
  bool mismatchCore = false;
  bool mismatchOverlap = false;
  mismatchCore = compare(jeSimMap, jeMap, errorsJEM, overlap);
  if (jetElementOvTES) {
    overlap = true;
    mismatchOverlap = compare(jeSimMap, ovMap, errorsJEM, overlap);
  }
  jeSimMap.clear();
  delete jetElementSIM;

  // Compare RoIs simulated from Jet Elements with JEM RoIs from data

  JemRoiCollection* jemRoiSIM = 0;
  if (jetElementTES || jetElementOvTES) {
    jemRoiSIM = new JemRoiCollection;
    if (mismatchCore || mismatchOverlap) {
      simulate(jetElementTES, jetElementOvTES, jemRoiSIM);
    } else {
      simulate(jetElementTES, jemRoiSIM);
    }
  }
  JemRoiMap jrSimMap;
  setupMap(jemRoiSIM, jrSimMap);
  compare(jrSimMap, jrMap, errorsJEM);
  jrSimMap.clear();
  delete jemRoiSIM;

  // Compare CMX-Jet TOBS simulated from RoIs with TOBs from data

  CmxJetTobCollection* cmxTobSIM = 0;
  if (jemRoiTES) {
    cmxTobSIM = new CmxJetTobCollection;
    simulate(jemRoiTES, cmxTobSIM);
  }
  CmxJetTobMap cmxTobSimMap;
  setupMap(cmxTobSIM, cmxTobSimMap);
  compare(cmxTobSimMap, ctMap, errorsJEM, errorsCMX);
  cmxTobSimMap.clear();
  delete cmxTobSIM;

  // Compare Local sums simulated from CMX TOBs with Local sums from data

  CmxJetHitsCollection* cmxLocalSIM = 0;
  if (cmxJetTobTES) {
    cmxLocalSIM = new CmxJetHitsCollection;
    simulate(cmxJetTobTES, cmxLocalSIM, LVL1::CMXJetHits::LOCAL_MAIN);
  }
  CmxJetHitsMap cmxLocalSimMap;
  setupMap(cmxLocalSIM, cmxLocalSimMap);
  compare(cmxLocalSimMap, chMap, errorsCMX, LVL1::CMXJetHits::LOCAL_MAIN);
  cmxLocalSimMap.clear();
  delete cmxLocalSIM;

  // Compare Local sums with Remote sums from data

  compare(chMap, chMap, errorsCMX, LVL1::CMXJetHits::REMOTE_MAIN);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmxJetHitsCollection* cmxTotalSIM = 0;
  if (cmxJetHitsTES) {
    cmxTotalSIM = new CmxJetHitsCollection;
    simulate(cmxJetHitsTES, cmxTotalSIM);
  }
  CmxJetHitsMap cmxTotalSimMap;
  setupMap(cmxTotalSIM, cmxTotalSimMap);
  compare(cmxTotalSimMap, chMap, errorsCMX, LVL1::CMXJetHits::TOTAL_MAIN);
  cmxTotalSimMap.clear();
  delete cmxTotalSIM;

  // Compare Topo Info simulated from CMX TOBs with Topo Info from data

  CmxJetHitsCollection* cmxTopoSIM = 0;
  if (cmxJetTobTES) {
    cmxTopoSIM = new CmxJetHitsCollection;
    simulate(cmxJetTobTES, cmxTopoSIM, LVL1::CMXJetHits::TOPO_CHECKSUM);
  }
  CmxJetHitsMap cmxTopoSimMap;
  setupMap(cmxTopoSIM, cmxTopoSimMap);
  compare(cmxTopoSimMap, chMap, errorsCMX, LVL1::CMXJetHits::TOPO_CHECKSUM);
  cmxTopoSimMap.clear();
  delete cmxTopoSIM;

  // Compare JEMEtSums simulated from JetElements with JEMEtSums from data

  JemEtSumsCollection* jemEtSumsSIM = 0;
  if (jetElementTES) {
    jemEtSumsSIM = new JemEtSumsCollection;
    simulate(jetElementTES, jemEtSumsSIM);
  }
  JemEtSumsMap jemEtSumsSimMap;
  setupMap(jemEtSumsSIM, jemEtSumsSimMap);
  compare(jemEtSumsSimMap, jsMap, errorsJEM);
  jemEtSumsSimMap.clear();
  delete jemEtSumsSIM;

  // Compare JEMEtSums with CMXEtSums from data

  compare(jsMap, csMap, errorsJEM, errorsCMX);

  // Compare Local sums simulated from CMXEtSums with Local sums from data

  CmxEtSumsCollection* cmxEtLocalSIM = 0;
  if (cmxEtSumsTES) {
    cmxEtLocalSIM = new CmxEtSumsCollection;
    simulate(cmxEtSumsTES, cmxEtLocalSIM, LVL1::CMXEtSums::LOCAL_STANDARD);
  }
  CmxEtSumsMap cmxEtLocalSimMap;
  setupMap(cmxEtLocalSIM, cmxEtLocalSimMap);
  compare(cmxEtLocalSimMap, csMap, errorsCMX, LVL1::CMXEtSums::LOCAL_STANDARD);
  cmxEtLocalSimMap.clear();
  delete cmxEtLocalSIM;

  // Compare Local Energy sums with Remote sums from data

  compare(csMap, csMap, errorsCMX, LVL1::CMXEtSums::REMOTE_STANDARD);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmxEtSumsCollection* cmxEtTotalSIM = 0;
  if (cmxEtSumsTES) {
    cmxEtTotalSIM = new CmxEtSumsCollection;
    simulate(cmxEtSumsTES, cmxEtTotalSIM, LVL1::CMXEtSums::TOTAL_STANDARD);
  }
  CmxEtSumsMap cmxEtTotalSimMap;
  setupMap(cmxEtTotalSIM, cmxEtTotalSimMap);
  compare(cmxEtTotalSimMap, csMap, errorsCMX, LVL1::CMXEtSums::TOTAL_STANDARD);
  cmxEtTotalSimMap.clear();
  delete cmxEtTotalSIM;

  // Compare Et Maps (sumEt/missingEt/missingEtSig) simulated from Total sums
  // with Et Maps from data

  CmxEtSumsCollection* cmxSumEtSIM = 0;
  if (cmxEtSumsTES) {
    cmxSumEtSIM = new CmxEtSumsCollection;
    simulate(cmxEtSumsTES, cmxSumEtSIM, LVL1::CMXEtSums::SUM_ET_STANDARD);
  }
  CmxEtSumsMap cmxSumEtSimMap;
  setupMap(cmxSumEtSIM, cmxSumEtSimMap);
  compare(cmxSumEtSimMap, csMap, errorsCMX, LVL1::CMXEtSums::SUM_ET_STANDARD);
  cmxSumEtSimMap.clear();
  delete cmxSumEtSIM;

  // Compare Total Energy sums and Et Maps with Energy RoIs from data

  compare(csMap, cmxRoiTES, errorsCMX);

  // Update error summary plots

  ErrorVector crateErr(s_crates);
  const int jemBins = s_crates * s_modules;
  const int cmxBins = s_crates * s_cmxs;
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < jemBins; ++loc) {
      if ((errorsJEM[loc] >> err) & 0x1) {
        m_h_jem_2d_SimEqDataOverview->Fill(loc, err, 1.);
      }
      if ((errorsJEM[loc + jemBins] >> err) & 0x1) {
        m_h_jem_2d_SimNeDataOverview->Fill(loc, err, 1.);
	error = 1;
	crateErr[loc/s_modules] |= (1 << err);
	fillEventSample(err, loc, true);
      }
      if (loc < cmxBins) {
        if ((errorsCMX[loc] >> err) & 0x1) {
          m_h_jem_2d_SimEqDataOverview->Fill(loc+jemBins, err, 1.);
        }
        if ((errorsCMX[loc + cmxBins] >> err) & 0x1) {
          m_h_jem_2d_SimNeDataOverview->Fill(loc+jemBins, err, 1.);
	  error = 1;
	  crateErr[loc/s_cmxs] |= (1 << err);
	  fillEventSample(err, loc, false);
        }
      }
    }
    if (error) m_h_jem_1d_SimNeDataSummary->Fill(err);
  }

  // Save error vector for global summary

  ErrorVector* save = new ErrorVector(crateErr);
  sc = evtStore()->record(save, m_errorLocation);
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording JEM mismatch vector in TES "
                    << endreq;
    return sc;
  }

  if (m_debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endreq;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endreq;

  if (endOfEventsBlock || endOfLumiBlock || endOfRun) {
  }

  return StatusCode::SUCCESS;
}

//  Compare Simulated JetElements with data

bool JEPSimMon::compare(const JetElementMap& jeSimMap,
                        const JetElementMap& jeMap, ErrorVector& errors,
			bool overlap)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated JetElements with data" << endreq;
  }

  bool mismatch = false;

  JetElementMap::const_iterator simMapIter    = jeSimMap.begin();
  JetElementMap::const_iterator simMapIterEnd = jeSimMap.end();
  JetElementMap::const_iterator datMapIter    = jeMap.begin();
  JetElementMap::const_iterator datMapIterEnd = jeMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    int simEm  = 0;
    int simHad = 0;
    int datEm  = 0;
    int datHad = 0;
    double eta = 0.;
    double phi = 0.;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated JetElement but no data JetElement

      const LVL1::JetElement* je = simMapIter->second;
      simEm  = je->emEnergy();
      simHad = je->hadEnergy();
      eta = je->eta();
      phi = je->phi();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data JetElement but no simulated JetElement

      const LVL1::JetElement* je = datMapIter->second;
      datEm  = je->emEnergy();
      datHad = je->hadEnergy();
      eta = je->eta();
      phi = je->phi();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::JetElement* jeS = simMapIter->second;
      const LVL1::JetElement* jeD = datMapIter->second;
      simEm  = jeS->emEnergy();
      simHad = jeS->hadEnergy();
      datEm  = jeD->emEnergy();
      datHad = jeD->hadEnergy();
      eta = jeD->eta();
      phi = jeD->phi();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEm && !simHad && !datEm && !datHad) continue;
    
    //  Fill in error vectors

    const LVL1::Coordinate coord(phi, eta);
    LVL1::CoordToHardware converter;
    const int crate = (overlap) ? converter.jepCrateOverlap(coord)
                                : converter.jepCrate(coord);
    const int jem   = (overlap) ? converter.jepModuleOverlap(coord)
                                : converter.jepModule(coord);
    if (crate >= s_crates || jem >= s_modules) continue;
    const int loc   = crate * s_modules + jem;
    const int jemBins = s_crates * s_modules;
    const int bitEm  = (1 << EMElementMismatch);
    const int bitHad = (1 << HadElementMismatch);
    if (simEm && simEm == datEm)    errors[loc] |= bitEm;
    if (simHad && simHad == datHad) errors[loc] |= bitHad;
    if (simEm != datEm)     errors[loc+jemBins] |= bitEm;
    if (simHad != datHad)   errors[loc+jemBins] |= bitHad;
    if (simEm != datEm || simHad != datHad) {
      mismatch = true;
      if (m_debug) {
        msg(MSG::VERBOSE) << "JE mismatch, EM data/sim: " << datEm << "/"
              << simEm << " Had data/sim: " << datHad << "/" << simHad
	      << " crate/jem: " << crate << "/" << jem
	      << " eta/phi: " << eta << "/" << phi
	      << endreq;
      }
    }
    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (overlap) {
      if (simEm && simEm == datEm)              hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap;
      if (simEm && datEm && simEm != datEm)     hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap;
      if (simEm && !datEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap;
      if (datEm && !simEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim;
      if (simHad && simHad == datHad)           hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap;
      if (simHad && datHad && simHad != datHad) hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap;
      if (simHad && !datHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap;
      if (datHad && !simHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim;
    } else {
      if (simEm && simEm == datEm)              hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqCore;
      if (simEm && datEm && simEm != datEm)     hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeCore;
      if (simEm && !datEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoCore;
      if (datEm && !simEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim;
      if (simHad && simHad == datHad)           hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqCore;
      if (simHad && datHad && simHad != datHad) hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeCore;
      if (simHad && !datHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoCore;
      if (datHad && !simHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim;
    }
    if (hist1) m_histTool->fillJEMEtaVsPhi(hist1, eta, phi);
    if (hist2) m_histTool->fillJEMEtaVsPhi(hist2, eta, phi);
  }

  return mismatch;
}

//  Compare Simulated RoIs with data

void JEPSimMon::compare(const JemRoiMap& roiSimMap,
                        const JemRoiMap& roiMap,
                              ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare Simulated RoIs with data" << endreq;

  LVL1::JEPRoIDecoder decoder;
  JemRoiMap::const_iterator simMapIter    = roiSimMap.begin();
  JemRoiMap::const_iterator simMapIterEnd = roiSimMap.end();
  JemRoiMap::const_iterator datMapIter    = roiMap.begin();
  JemRoiMap::const_iterator datMapIterEnd = roiMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simEnLg = 0;
    unsigned int simEnSm = 0;
    unsigned int datEnLg = 0;
    unsigned int datEnSm = 0;
    const LVL1::JEMTobRoI* roi = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                     ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated RoI but no data RoI

      roi = simMapIter->second;
      simEnLg = roi->energyLarge();
      simEnSm = roi->energySmall();
      ++simMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Sim  RoI crate/jem/frame/loc/energyLg/energySm: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->energyLarge() << "/"
	    << roi->energySmall() << endreq;
      }

    } else if ((simMapIter == simMapIterEnd) ||
                     ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data RoI but no simulated RoI

      roi = datMapIter->second;
      datEnLg = roi->energyLarge();
      datEnSm = roi->energySmall();
      ++datMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Data RoI crate/jem/frame/loc/energyLg/energySm: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->energyLarge() << "/"
	    << roi->energySmall() << endreq;
      }

    } else {

      // Have both

      const LVL1::JEMTobRoI* roiS = simMapIter->second;
      roi     = datMapIter->second;
      simEnLg = roiS->energyLarge();
      simEnSm = roiS->energySmall();
      datEnLg = roi->energyLarge();
      datEnSm = roi->energySmall();
      ++simMapIter;
      ++datMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Sim  RoI crate/jem/frame/loc/energyLg/energySm: "
            << roiS->crate() << "/" << roiS->jem() << "/" << roiS->frame() << "/"
	    << roiS->location() << "/" << roiS->energyLarge() << "/"
	    << roiS->energySmall() << endreq;
        msg(MSG::VERBOSE)
            << "Data RoI crate/jem/frame/loc/energyLg/energySm: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->energyLarge() << "/"
	    << roi->energySmall() << endreq;
      }
    }

    if (!simEnLg && !simEnSm && !datEnLg && !datEnSm) continue;

    //  Check LimitedRoISet bit

    const int crate = roi->crate();
    if (!datEnLg && !datEnSm && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int jem   = roi->jem();
    const int frame = roi->frame();
    const int local = roi->location();
    const int locX  = crate * s_modules + jem;
    const int locY  = frame * s_locCoords + local;
    const int jemBins = s_crates * s_modules;
    const int bit = (1 << RoIMismatch);
    const LVL1::CoordinateRange coord(decoder.coordinate(roi->roiWord()));    // Should still work
    double eta = coord.eta();
    // Distinguish right forward columns 3 and 4 for checking purposes
    if ((jem == 7 || jem == 15) && frame > 3) eta = (local%2) ? 4.05 : 3.2;
    const double phi = coord.phi();

    TH2F_LW* hist = 0;
    if (simEnLg && simEnLg == datEnLg) {
      errors[locX] |= bit;
      hist = m_h_jem_2d_roi_EnergyLgSimEqData;
    } else if (simEnLg != datEnLg) {
      errors[locX+jemBins] |= bit;
      if (simEnLg && datEnLg) {
        hist = m_h_jem_2d_roi_EnergyLgSimNeData;
      } else if (!datEnLg) {
        hist = m_h_jem_2d_roi_EnergyLgSimNoData;
      } else {
        hist = m_h_jem_2d_roi_EnergyLgDataNoSim;
      }
    }
    if (hist) hist->Fill(locX, locY);

    hist = 0;
    if (simEnSm && simEnSm == datEnSm) {
      errors[locX] |= bit;
      hist = m_h_jem_2d_roi_EnergySmSimEqData;
    } else if (simEnSm != datEnSm) {
      errors[locX+jemBins] |= bit;
      if (simEnSm && datEnSm) {
        hist = m_h_jem_2d_roi_EnergySmSimNeData;
      } else if (!datEnSm) {
        hist = m_h_jem_2d_roi_EnergySmSimNoData;
      } else {
        hist = m_h_jem_2d_roi_EnergySmDataNoSim;
      }
    }
    if (hist) hist->Fill(locX, locY);

    // do we need this?
    hist = 0;
    if (simEnLg == datEnLg && simEnSm == datEnSm) {
      hist = m_h_jem_2d_etaPhi_roi_SimEqData;
    } else {
      if ((simEnLg || simEnSm) && (datEnLg || datEnSm)) {
        hist = m_h_jem_2d_etaPhi_roi_SimNeData;
      } else if (!datEnLg && !datEnSm) {
        hist = m_h_jem_2d_etaPhi_roi_SimNoData;
      } else {
        hist = m_h_jem_2d_etaPhi_roi_DataNoSim;
      }
    }
    if (hist) m_histTool->fillJEMRoIEtaVsPhi(hist, eta, phi);

    if (m_debug && (simEnLg != datEnLg || simEnSm != datEnSm)) {
      msg(MSG::VERBOSE) << " RoI Mismatch Crate/JEM DataEnergy/SimEnergy: "
                        << crate << "/" << jem << " "
			<< datEnLg << "/" << simEnLg << " "
			<< datEnSm << "/" << simEnSm << endreq;
    }
  }
}

//  Compare simulated CMX TOBs with data

void JEPSimMon::compare(const CmxJetTobMap& simMap, const CmxJetTobMap& datMap,
                        ErrorVector& errorsJEM, ErrorVector& errorsCMX)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare simulated CMX TOBs with data" << endreq;

  LVL1::JEPRoIDecoder decoder;
  CmxJetTobMap::const_iterator simMapIter    = simMap.begin();
  CmxJetTobMap::const_iterator simMapIterEnd = simMap.end();
  CmxJetTobMap::const_iterator datMapIter    = datMap.begin();
  CmxJetTobMap::const_iterator datMapIterEnd = datMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey  = 0;
    int datKey  = 0;
    int simEnLg = 0;
    int simEnSm = 0;
    int datEnLg = 0;
    int datEnSm = 0;

    const LVL1::CMXJetTob* tob = 0;
    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                  ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated TOB but no data TOB

      tob = simMapIter->second;
      simEnLg = tob->energyLarge();
      simEnSm = tob->energySmall();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                  ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data TOB but no simulated TOB

      tob = datMapIter->second;
      datEnLg = tob->energyLarge();
      datEnSm = tob->energySmall();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::CMXJetTob* tobS = simMapIter->second;
      tob = datMapIter->second;
      simEnLg = tobS->energyLarge();
      simEnSm = tobS->energySmall();
      datEnLg = tob->energyLarge();
      datEnSm = tob->energySmall();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEnLg && !simEnSm && !datEnLg && !datEnSm) continue;

    //  Check LimitedRoISet bit

    const int crate = tob->crate();
    if (!simEnLg && !simEnSm && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int cmx   = 1;                                              //<<== CHECK
    const int jem   = tob->jem();
    const int frame = tob->frame();
    const int loc   = tob->location();
    const int locX  = crate * s_modules + jem;
    const int locY  = frame * 4 + loc;
    const int loc2  = crate * s_cmxs + cmx;
    const int jemBins = s_crates * s_modules;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << CMXJetTobMismatch);
    const uint32_t roiWord = ((((((crate<<4)+jem)<<3)+frame)<<2)+loc)<<19;
    const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
    const double eta = coord.eta();
    const double phi = coord.phi();

    TH2F_LW* hist = 0;
    if (simEnLg || datEnLg) {
      if (simEnLg == datEnLg) {
        errorsJEM[locX] |= bit;
        errorsCMX[loc2] |= bit;
        hist = m_h_cmx_2d_tob_EnergyLgSimEqData;
      } else {
        errorsJEM[locX+jemBins] |= bit;
        errorsCMX[loc2+cmxBins] |= bit;
        if (simEnLg && datEnLg) {
          hist = m_h_cmx_2d_tob_EnergyLgSimNeData;
        } else if (simEnLg && !datEnLg) {
          hist = m_h_cmx_2d_tob_EnergyLgSimNoData;
        } else {
          hist = m_h_cmx_2d_tob_EnergyLgDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    hist = 0;
    if (simEnSm || datEnSm) {
      if (simEnSm == datEnSm) {
        errorsJEM[locX] |= bit;
        errorsCMX[loc2] |= bit;
        hist = m_h_cmx_2d_tob_EnergySmSimEqData;
      } else {
        errorsJEM[locX+jemBins] |= bit;
        errorsCMX[loc2+cmxBins] |= bit;
        if (simEnSm && datEnSm) {
          hist = m_h_cmx_2d_tob_EnergySmSimNeData;
        } else if (simEnSm && !datEnSm) {
          hist = m_h_cmx_2d_tob_EnergySmSimNoData;
        } else {
          hist = m_h_cmx_2d_tob_EnergySmDataNoSim;
        }
      }
      if (hist) hist->Fill(locX, locY);
    }
    // do we need this?
    hist = 0;
    if (simEnLg == datEnLg && simEnSm == datEnSm) {
      hist = m_h_cmx_2d_etaPhi_tob_SimEqData;
    } else {
      if ((simEnLg || simEnSm) && (datEnLg || datEnSm)) {
        hist = m_h_cmx_2d_etaPhi_tob_SimNeData;
      } else if (!datEnLg && !datEnSm) {
        hist = m_h_cmx_2d_etaPhi_tob_SimNoData;
      } else {
        hist = m_h_cmx_2d_etaPhi_tob_DataNoSim;
      }
    }
    if (hist) m_histTool->fillJEMRoIEtaVsPhi(hist, eta, phi);

  }
}

//  Compare Simulated CMX Hit Sums and Data CMX Hit Sums

void JEPSimMon::compare(const CmxJetHitsMap& cmxSimMap,
                        const CmxJetHitsMap& cmxMap,
                              ErrorVector& errors, int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated CMX Hit Sums and Data CMX Hit Sums"
                    << endreq;
  }

  const bool local  = (selection == LVL1::CMXJetHits::LOCAL_MAIN);
  const bool remote = (selection == LVL1::CMXJetHits::REMOTE_MAIN);
  const bool total  = (selection == LVL1::CMXJetHits::TOTAL_MAIN);
  const bool topo   = (selection == LVL1::CMXJetHits::TOPO_CHECKSUM);
  if (!local && !remote && !total && !topo) return;
  unsigned int hitsSimMain0 = 0;
  unsigned int hitsSimMain1 = 0;
  unsigned int hitsSimFwd0  = 0;
  unsigned int hitsSimFwd1  = 0;
  unsigned int hitsDatMain0 = 0;
  unsigned int hitsDatMain1 = 0;
  unsigned int hitsDatFwd0  = 0;
  unsigned int hitsDatFwd1  = 0;
  int overflowSim = 0;
  int overflowDat = 0;
  CmxJetHitsMap::const_iterator cmxSimMapIter    = cmxSimMap.begin();
  CmxJetHitsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
  CmxJetHitsMap::const_iterator cmxMapIter       = cmxMap.begin();
  CmxJetHitsMap::const_iterator cmxMapIterEnd    = cmxMap.end();

  while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd) {

    int cmxSimKey = 0;
    int cmxKey    = 0;
    unsigned int cmxSimHit0 = 0;
    unsigned int cmxSimHit1 = 0;
    unsigned int cmxHit0 = 0;
    unsigned int cmxHit1 = 0;
    int cmxSimOvf = 0;
    int cmxOvf    = 0;
    int crate  = 0;
    int source = 0;

    if (cmxSimMapIter != cmxSimMapIterEnd) cmxSimKey = cmxSimMapIter->first;
    if (cmxMapIter    != cmxMapIterEnd)    cmxKey    = cmxMapIter->first;

    if ((cmxMapIter == cmxMapIterEnd) ||
            ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey))) {

      // Sim CMX Hits but no Data CMX Hits

      const LVL1::CMXJetHits* cmxS = cmxSimMapIter->second;
      ++cmxSimMapIter;
      source = cmxS->source();
      if (local  && source != LVL1::CMXJetHits::LOCAL_MAIN &&
                    source != LVL1::CMXJetHits::LOCAL_FORWARD) continue;
      if (remote && source != LVL1::CMXJetHits::LOCAL_MAIN &&
                    source != LVL1::CMXJetHits::LOCAL_FORWARD) continue;
      if (total  && source != LVL1::CMXJetHits::TOTAL_MAIN &&
                    source != LVL1::CMXJetHits::TOTAL_FORWARD) continue;
      if (topo   && source != LVL1::CMXJetHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxSimHit0 = cmxS->hits0();
      cmxSimHit1 = cmxS->hits1();
      crate      = cmxS->crate();
      if (remote || total) {
        LVL1::DataError err(cmxS->error0());
	cmxSimOvf = err.get(LVL1::DataError::Overflow);
      }

    } else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
                 ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey))) {

      // Data CMX Hits but no Sim CMX Hits

      const LVL1::CMXJetHits* cmxD = cmxMapIter->second;
      ++cmxMapIter;
      source   = cmxD->source();
      if (local  && source != LVL1::CMXJetHits::LOCAL_MAIN &&
                    source != LVL1::CMXJetHits::LOCAL_FORWARD)  continue;
      if (remote && source != LVL1::CMXJetHits::REMOTE_MAIN &&
		    source != LVL1::CMXJetHits::REMOTE_FORWARD) continue;
      if (total  && source != LVL1::CMXJetHits::TOTAL_MAIN &&
                    source != LVL1::CMXJetHits::TOTAL_FORWARD)  continue;
      if (topo   && source != LVL1::CMXJetHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxHit0 = cmxD->hits0();
      cmxHit1 = cmxD->hits1();
      crate   = cmxD->crate();
      if (remote || total) {
        LVL1::DataError err(cmxD->error0());
	cmxOvf = err.get(LVL1::DataError::Overflow);
      }

   } else {

      // Have both

      const LVL1::CMXJetHits* cmxS = cmxSimMapIter->second;
      const LVL1::CMXJetHits* cmxD = cmxMapIter->second;
      ++cmxSimMapIter;
      ++cmxMapIter;
      source   = cmxS->source();
      if (local  && source != LVL1::CMXJetHits::LOCAL_MAIN    &&
                    source != LVL1::CMXJetHits::LOCAL_FORWARD)  continue;
      if (remote && source != LVL1::CMXJetHits::LOCAL_MAIN    &&
                    source != LVL1::CMXJetHits::LOCAL_FORWARD &&
                    source != LVL1::CMXJetHits::REMOTE_MAIN   &&
		    source != LVL1::CMXJetHits::REMOTE_FORWARD) continue;
      if (total  && source != LVL1::CMXJetHits::TOTAL_MAIN    &&
                    source != LVL1::CMXJetHits::TOTAL_FORWARD)  continue;
      if (topo   && source != LVL1::CMXJetHits::TOPO_CHECKSUM      &&
                    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP &&
		    source != LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS) continue;
      cmxSimHit0 = cmxS->hits0();
      cmxSimHit1 = cmxS->hits1();
      cmxHit0    = cmxD->hits0();
      cmxHit1    = cmxD->hits1();
      crate      = cmxS->crate();
      if (remote || total) {
        LVL1::DataError err(cmxS->error0());
	cmxSimOvf = err.get(LVL1::DataError::Overflow);
        LVL1::DataError err2(cmxD->error0());
	cmxOvf = err2.get(LVL1::DataError::Overflow);
      }
    }

    if (!cmxSimHit0 && !cmxSimHit1 && !cmxHit0 && !cmxHit1 &&
                                      !cmxSimOvf && !cmxOvf) continue;
    
    //  Fill in error plots

    if (local || total) {
      const int cmx = 1;                                              //<<== CHECK (and below)
      const int loc = crate * s_cmxs + cmx;
      const int cmxBins = s_crates * s_cmxs;
      const int bit = (local) ? (1 << LocalJetMismatch)
                              : (1 << TotalJetMismatch);
      TH1F_LW* hist = 0;
      if (cmxSimHit0 == cmxHit0 && cmxSimHit1 == cmxHit1) {
        errors[loc] |= bit;
	hist = m_h_cmx_1d_thresh_SumsSimEqData;
      } else {
        errors[loc+cmxBins] |= bit;
	if ((cmxSimHit0 || cmxSimHit1) && (cmxHit0 || cmxHit1))
	                               hist = m_h_cmx_1d_thresh_SumsSimNeData;
	else if (!cmxHit0 && !cmxHit1) hist = m_h_cmx_1d_thresh_SumsSimNoData;
	else                           hist = m_h_cmx_1d_thresh_SumsDataNoSim;
      }
      const int loc1 = (total) ? 3 : crate;
      if (hist) hist->Fill(loc1);

      if (total && (cmxSimOvf || cmxOvf) &&
                              source == LVL1::CMXJetHits::TOTAL_MAIN) {
        if (cmxSimOvf == cmxOvf) {
	  errors[loc] |= bit;
	  hist = m_h_cmx_1d_thresh_SumsOvfSimEqData;
        } else {
	  errors[loc+cmxBins] |= bit;
	  hist = m_h_cmx_1d_thresh_SumsOvfSimNeData;
        }
	if (hist) hist->Fill(1);
      }

      int nThresh0 = 5;
      int nThresh1 = 5;
      int thrLen   = 3;
      int offset0  = 0;
      int offset1  = 5;
      if (source == LVL1::CMXJetHits::LOCAL_FORWARD ||
          source == LVL1::CMXJetHits::TOTAL_FORWARD) {
	nThresh0 = 8;
	nThresh1 = 7;
        thrLen   = 2;
	offset0  = 10;
	offset1  = 18;
      }
      int same = m_histTool->thresholdsSame(cmxHit0, cmxSimHit0, nThresh0,
                                                                  thrLen);
      int diff = m_histTool->thresholdsDiff(cmxHit0, cmxSimHit0, nThresh0,
                                                                  thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh0, 1, offset0);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh0, 1, offset0);
      same = m_histTool->thresholdsSame(cmxHit1, cmxSimHit1, nThresh1, thrLen);
      diff = m_histTool->thresholdsDiff(cmxHit1, cmxSimHit1, nThresh1, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh1, 1, offset1);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh1, 1, offset1);
    } else if (topo) {
      const int cmx = 1;
      const int loc = crate * s_cmxs + cmx;
      const int cmxBins = s_crates * s_cmxs;
      const int bit = (1 << CMXJetTopoMismatch);
      TH2F_LW* hist = 0;
      if (cmxSimHit0 == cmxHit0 && cmxSimHit1 == cmxHit1) {
        errors[loc] |= bit;
	hist = m_h_cmx_2d_topo_SimEqData;
      } else {
        errors[loc+cmxBins] |= bit;
	if ((cmxSimHit0 || cmxSimHit1) && (cmxHit0 || cmxHit1))
	                               hist = m_h_cmx_2d_topo_SimNeData;
	else if (!cmxHit0 && !cmxHit1) hist = m_h_cmx_2d_topo_SimNoData;
	else                           hist = m_h_cmx_2d_topo_DataNoSim;
      }
      const int loc1 = (source == LVL1::CMXJetHits::TOPO_CHECKSUM)
                       ? 0 : (source == LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP)
		             ? 1 : 2;
      if (hist) hist->Fill(crate, loc1);
    } else {
      if (source == LVL1::CMXJetHits::LOCAL_MAIN) {
        if (crate == 0) {
	  hitsSimMain0 = cmxSimHit0;
	  hitsSimMain1 = cmxSimHit1;
	  overflowSim  = cmxSimOvf;
        }
      } else if (source == LVL1::CMXJetHits::LOCAL_FORWARD) {
        if (crate == 0) {
	  hitsSimFwd0  = cmxSimHit0;
	  hitsSimFwd1  = cmxSimHit1;
        }
      } else if (source == LVL1::CMXJetHits::REMOTE_MAIN) {
          hitsDatMain0 = cmxHit0;
          hitsDatMain1 = cmxHit1;
	  overflowDat  = cmxOvf;
      } else {
          hitsDatFwd0  = cmxHit0;
          hitsDatFwd1  = cmxHit1;
      }
    }
  }
  if (remote) {
    const int crate = 1;
    const int cmx = 1;
    const int loc = crate * s_cmxs + cmx;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << RemoteJetMismatch);
    const int loc1 = 2;

    TH1F_LW* hist = 0;
    if (hitsSimMain0 || hitsSimMain1 || hitsDatMain0 || hitsDatMain1) {
      if (hitsSimMain0 == hitsDatMain0 && hitsSimMain1 == hitsDatMain1) {
        errors[loc] |= bit;
        hist = m_h_cmx_1d_thresh_SumsSimEqData;
      } else {
        errors[loc+cmxBins] |= bit;
        if ((hitsSimMain0 || hitsSimMain1) && (hitsDatMain0 || hitsDatMain1))
	     hist = m_h_cmx_1d_thresh_SumsSimNeData;
        else if (!hitsDatMain0 && !hitsDatMain1)
	     hist = m_h_cmx_1d_thresh_SumsSimNoData;
        else hist = m_h_cmx_1d_thresh_SumsDataNoSim;
      }
      if (hist) hist->Fill(loc1);
    }
    if (hitsSimFwd0 || hitsSimFwd1 || hitsDatFwd0 || hitsDatFwd1) {
      hist = 0;
      if (hitsSimFwd0 == hitsDatFwd0 && hitsSimFwd1 == hitsDatFwd1) {
        errors[loc] |= bit;
        hist = m_h_cmx_1d_thresh_SumsSimEqData;
      } else {
        errors[loc+cmxBins] |= bit;
        if ((hitsSimFwd0 || hitsSimFwd1) && (hitsDatFwd0 || hitsDatFwd1))
	     hist = m_h_cmx_1d_thresh_SumsSimNeData;
        else if (!hitsDatFwd0 && !hitsDatFwd1)
	     hist = m_h_cmx_1d_thresh_SumsSimNoData;
        else hist = m_h_cmx_1d_thresh_SumsDataNoSim;
      }
      if (hist) hist->Fill(loc1);
    }
    if (overflowSim || overflowDat) {
      hist = 0;
      if (overflowSim == overflowDat) {
        errors[loc] |= bit;
	hist = m_h_cmx_1d_thresh_SumsOvfSimEqData;
      } else {
	errors[loc+cmxBins] |= bit;
	hist = m_h_cmx_1d_thresh_SumsOvfSimNeData;
      }
      if (hist) hist->Fill(0);
    }

    const int mainLen = 3;
    const int fwdLen  = 2;
    int nThresh = 5;
    int offset = nThresh;
    int same = m_histTool->thresholdsSame(hitsDatMain0, hitsSimMain0, nThresh,
                                                                    mainLen);
    int diff = m_histTool->thresholdsDiff(hitsDatMain0, hitsSimMain0, nThresh,
                                                                    mainLen);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                                      loc1, same, nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                                      loc1, diff, nThresh, 1);
    same = m_histTool->thresholdsSame(hitsDatMain1, hitsSimMain1, nThresh,
                                                                    mainLen);
    diff = m_histTool->thresholdsDiff(hitsDatMain1, hitsSimMain1, nThresh,
                                                                    mainLen);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh, 1, offset);
    offset += nThresh;
    nThresh = 8;
    same = m_histTool->thresholdsSame(hitsDatFwd0, hitsSimFwd0, nThresh, fwdLen);
    diff = m_histTool->thresholdsDiff(hitsDatFwd0, hitsSimFwd0, nThresh, fwdLen);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh, 1, offset);
    offset += nThresh;
    nThresh = 7;
    same = m_histTool->thresholdsSame(hitsDatFwd1, hitsSimFwd1, nThresh, fwdLen);
    diff = m_histTool->thresholdsDiff(hitsDatFwd1, hitsSimFwd1, nThresh, fwdLen);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh, 1, offset);
  }
}

//  Compare simulated JEM Et Sums with data

void JEPSimMon::compare(const JemEtSumsMap& jemSimMap,
                        const JemEtSumsMap& jemMap,
                              ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare simulated JEM Et Sums with data"
                               << endreq;

  JemEtSumsMap::const_iterator simMapIter    = jemSimMap.begin();
  JemEtSumsMap::const_iterator simMapIterEnd = jemSimMap.end();
  JemEtSumsMap::const_iterator datMapIter    = jemMap.begin();
  JemEtSumsMap::const_iterator datMapIterEnd = jemMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simEt = 0;
    unsigned int simEx = 0;
    unsigned int simEy = 0;
    unsigned int datEt = 0;
    unsigned int datEx = 0;
    unsigned int datEy = 0;
    int crate = 0;
    int jem   = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
               ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulation EtSums but no data EtSums

      const LVL1::JEMEtSums* sime = simMapIter->second;
      simEt = sime->Et();
      simEx = sime->Ex();
      simEy = sime->Ey();
      crate = sime->crate();
      jem   = sime->module();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data EtSums but no simulation EtSums

      const LVL1::JEMEtSums* date = datMapIter->second;
      datEt = date->Et();
      datEx = date->Ex();
      datEy = date->Ey();
      crate = date->crate();
      jem   = date->module();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::JEMEtSums* sime = simMapIter->second;
      const LVL1::JEMEtSums* date = datMapIter->second;
      simEt = sime->Et();
      simEx = sime->Ex();
      simEy = sime->Ey();
      datEt = date->Et();
      datEx = date->Ex();
      datEy = date->Ey();
      crate = date->crate();
      jem   = date->module();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEt && !simEx && !simEy && !datEt && !datEx && !datEy) continue;
    
    //  Fill in error vector

    const int loc = crate * s_modules + jem;
    const int jemBins = s_crates * s_modules;
    const int bit = (1 << JEMEtSumsMismatch);
    if (simEt == datEt && simEx == datEx && simEy == datEy) errors[loc] |= bit;
    if (simEt != datEt || simEx != datEx || simEy != datEy)
                                                    errors[loc+jemBins] |= bit;
    if (m_debug && (simEt != datEt || simEx != datEx || simEy != datEy)) {
      msg(MSG::VERBOSE)
            << "EtSums Mismatch Crate/JEM, Data Et/Ex/Ey, Sim Et/Ex/Ey: "
            << crate << "/" << jem << ", "
            << datEt << "/" << datEx << "/" << datEy << ", "
	    << simEt << "/" << simEx << "/" << simEy << endreq;
    }
    TH2F_LW* hist = 0;
    if (simEx && simEx == datEx) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEx != datEx) {
      if (simEx && datEx) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEx)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 0);
    hist = 0;
    if (simEy && simEy == datEy) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEy != datEy) {
      if (simEy && datEy) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEy)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 1);
    hist = 0;
    if (simEt && simEt == datEt) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEt != datEt) {
      if (simEt && datEt) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEt)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 2);
  }
}

//  Compare JEM EtSums and CMX EtSums

void JEPSimMon::compare(const JemEtSumsMap& jemMap,
                        const CmxEtSumsMap& cmxMap,
                              ErrorVector& errorsJEM,
			      ErrorVector& errorsCMX)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare JEM EtSums and CMX EtSums" << endreq;

  JemEtSumsMap::const_iterator jemMapIter    = jemMap.begin();
  JemEtSumsMap::const_iterator jemMapIterEnd = jemMap.end();
  CmxEtSumsMap::const_iterator cmxMapIter    = cmxMap.begin();
  CmxEtSumsMap::const_iterator cmxMapIterEnd = cmxMap.end();

  while (jemMapIter != jemMapIterEnd || cmxMapIter != cmxMapIterEnd) {

    int jemKey = 0;
    int cmxKey = 0;
    unsigned int jemEt = 0;
    unsigned int jemEx = 0;
    unsigned int jemEy = 0;
    unsigned int cmxEt = 0;
    unsigned int cmxEx = 0;
    unsigned int cmxEy = 0;
    int crate  = 0;
    int jem    = 0;

    if (jemMapIter != jemMapIterEnd) jemKey = jemMapIter->first;
    if (cmxMapIter != cmxMapIterEnd) cmxKey = cmxMapIter->first;

    if ((cmxMapIter == cmxMapIterEnd) ||
                ((jemMapIter != jemMapIterEnd) && (cmxKey > jemKey))) {

      // JEM EtSums but no CMX EtSums

      const LVL1::JEMEtSums* jeme = jemMapIter->second;
      jemEt = jeme->Et();
      jemEx = jeme->Ex();
      jemEy = jeme->Ey();
      crate = jeme->crate();
      jem   = jeme->module();
      ++jemMapIter;

    } else if ((jemMapIter == jemMapIterEnd) ||
                ((cmxMapIter != cmxMapIterEnd) && (jemKey > cmxKey))) {

      // CMX EtSums but no JEM EtSums

      const LVL1::CMXEtSums* cmxe = cmxMapIter->second;
      cmxEt = cmxe->Et();
      cmxEx = cmxe->Ex();
      cmxEy = cmxe->Ey();
      crate = cmxe->crate();
      jem   = cmxe->source();
      ++cmxMapIter;
      if (jem > 15) continue;

    } else {

      // Have both

      const LVL1::JEMEtSums* jeme = jemMapIter->second;
      const LVL1::CMXEtSums* cmxe = cmxMapIter->second;
      jemEt = jeme->Et();
      jemEx = jeme->Ex();
      jemEy = jeme->Ey();
      cmxEt = cmxe->Et();
      cmxEx = cmxe->Ex();
      cmxEy = cmxe->Ey();
      crate = jeme->crate();
      jem   = jeme->module();
      ++jemMapIter;
      ++cmxMapIter;
    }

    if (!jemEt && !jemEx && !jemEy && !cmxEt && !cmxEx && !cmxEy) continue;
    
    //  Fill in error vectors

    const int cmx = 0; // CMX-Energy is left ==> cmx 0                        << CHECK
    const int loc  = crate * s_modules + jem;
    const int loc2 = crate * s_cmxs + cmx;
    const int jemBins = s_crates * s_modules;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << CMXEtSumsMismatch);
    if (jemEt == cmxEt && jemEx == cmxEx && jemEy == cmxEy) {
      errorsJEM[loc]  |= bit;
      errorsCMX[loc2] |= bit;
    } else {
      errorsJEM[loc+jemBins]  |= bit;
      errorsCMX[loc2+cmxBins] |= bit;
    }
    TH2F_LW* hist = 0;
    if (jemEx && jemEx == cmxEx) hist = m_h_cmx_2d_energy_JemEqCmx;
    else if (jemEx != cmxEx) {
      if (jemEx && cmxEx) hist = m_h_cmx_2d_energy_JemNeCmx;
      else if (!cmxEx)    hist = m_h_cmx_2d_energy_JemNoCmx;
      else                hist = m_h_cmx_2d_energy_CmxNoJem;
    }
    if (hist) hist->Fill(loc, 0);
    hist = 0;
    if (jemEy && jemEy == cmxEy) hist = m_h_cmx_2d_energy_JemEqCmx;
    else if (jemEy != cmxEy) {
      if (jemEy && cmxEy) hist = m_h_cmx_2d_energy_JemNeCmx;
      else if (!cmxEy)    hist = m_h_cmx_2d_energy_JemNoCmx;
      else                hist = m_h_cmx_2d_energy_CmxNoJem;
    }
    if (hist) hist->Fill(loc, 1);
    hist = 0;
    if (jemEt && jemEt == cmxEt) hist = m_h_cmx_2d_energy_JemEqCmx;
    else if (jemEt != cmxEt) {
      if (jemEt && cmxEt) hist = m_h_cmx_2d_energy_JemNeCmx;
      else if (!cmxEt)    hist = m_h_cmx_2d_energy_JemNoCmx;
      else                hist = m_h_cmx_2d_energy_CmxNoJem;
    }
    if (hist) hist->Fill(loc, 2);
  }
}

//  Compare Simulated CMX EtSums and Data CMX EtSums

void JEPSimMon::compare(const CmxEtSumsMap& cmxSimMap,
                        const CmxEtSumsMap& cmxMap,
                              ErrorVector& errors, int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated CMX EtSums and Data CMX EtSums"
                    << endreq;
  }

  const bool local  = (selection == LVL1::CMXEtSums::LOCAL_STANDARD);
  const bool remote = (selection == LVL1::CMXEtSums::REMOTE_STANDARD);
  const bool total  = (selection == LVL1::CMXEtSums::TOTAL_STANDARD);
  const bool etmaps = (selection == LVL1::CMXEtSums::SUM_ET_STANDARD);
  if (!local && !remote && !total && !etmaps) return;
  unsigned int localEt   = 0;
  unsigned int localEx   = 0;
  unsigned int localEy   = 0;
  unsigned int localEtM  = 0;
  unsigned int localExM  = 0;
  unsigned int localEyM  = 0;
  unsigned int remoteEt  = 0;
  unsigned int remoteEx  = 0;
  unsigned int remoteEy  = 0;
  unsigned int remoteEtM = 0;
  unsigned int remoteExM = 0;
  unsigned int remoteEyM = 0;
  CmxEtSumsMap::const_iterator cmxSimMapIter    = cmxSimMap.begin();
  CmxEtSumsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
  CmxEtSumsMap::const_iterator cmxMapIter       = cmxMap.begin();
  CmxEtSumsMap::const_iterator cmxMapIterEnd    = cmxMap.end();

  while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd) {

    int cmxSimKey = 0;
    int cmxKey    = 0;
    unsigned int cmxSimEt = 0;
    unsigned int cmxSimEx = 0;
    unsigned int cmxSimEy = 0;
    unsigned int cmxEt = 0;
    unsigned int cmxEx = 0;
    unsigned int cmxEy = 0;
    int crate  = 0;
    int source = 0;

    if (cmxSimMapIter != cmxSimMapIterEnd) cmxSimKey = cmxSimMapIter->first;
    if (cmxMapIter    != cmxMapIterEnd)    cmxKey    = cmxMapIter->first;

    if ((cmxMapIter == cmxMapIterEnd) ||
          ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey))) {

      // Sim CMX EtSums but no Data CMX EtSums

      const LVL1::CMXEtSums* cmxS = cmxSimMapIter->second;
      ++cmxSimMapIter;
      source = cmxS->source();
      if (local  && source != LVL1::CMXEtSums::LOCAL_STANDARD &&
                    source != LVL1::CMXEtSums::LOCAL_RESTRICTED) continue;
      if (remote && source != LVL1::CMXEtSums::LOCAL_STANDARD &&
                    source != LVL1::CMXEtSums::LOCAL_RESTRICTED) continue;
      if (total  && source != LVL1::CMXEtSums::TOTAL_STANDARD &&
                    source != LVL1::CMXEtSums::TOTAL_RESTRICTED) continue;
      if (etmaps && source != LVL1::CMXEtSums::SUM_ET_STANDARD &&
                    source != LVL1::CMXEtSums::SUM_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_STANDARD &&
                    source != LVL1::CMXEtSums::MISSING_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD) continue;
      cmxSimEt = cmxS->Et();
      cmxSimEx = cmxS->Ex();
      cmxSimEy = cmxS->Ey();
      if (!etmaps) { // include overflow bit in test
        cmxSimEt |= ((cmxS->EtError() & 0x1) << 15);
        cmxSimEx |= ((cmxS->ExError() & 0x1) << 15);
        cmxSimEy |= ((cmxS->EyError() & 0x1) << 15);
      }
      crate    = cmxS->crate();

    } else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
                ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey))) {

      // Data CMX EtSums but no Sim CMX EtSums

      const LVL1::CMXEtSums* cmxD = cmxMapIter->second;
      ++cmxMapIter;
      source   = cmxD->source();
      if (local  && source != LVL1::CMXEtSums::LOCAL_STANDARD &&
                    source != LVL1::CMXEtSums::LOCAL_RESTRICTED)  continue;
      if (remote && source != LVL1::CMXEtSums::REMOTE_STANDARD &&
                    source != LVL1::CMXEtSums::REMOTE_RESTRICTED) continue;
      if (total  && source != LVL1::CMXEtSums::TOTAL_STANDARD &&
                    source != LVL1::CMXEtSums::TOTAL_RESTRICTED)  continue;
      if (etmaps && source != LVL1::CMXEtSums::SUM_ET_STANDARD &&
                    source != LVL1::CMXEtSums::SUM_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_STANDARD &&
                    source != LVL1::CMXEtSums::MISSING_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD) continue;
      cmxEt = cmxD->Et();
      cmxEx = cmxD->Ex();
      cmxEy = cmxD->Ey();
      if (!etmaps) {
        cmxEt |= ((cmxD->EtError() & 0x1) << 15);
        cmxEx |= ((cmxD->ExError() & 0x1) << 15);
        cmxEy |= ((cmxD->EyError() & 0x1) << 15);
      }
      crate = cmxD->crate();

    } else {

      // Have both

      const LVL1::CMXEtSums* cmxS = cmxSimMapIter->second;
      const LVL1::CMXEtSums* cmxD = cmxMapIter->second;
      ++cmxSimMapIter;
      ++cmxMapIter;
      source   = cmxS->source();
      if (local  && source != LVL1::CMXEtSums::LOCAL_STANDARD &&
                    source != LVL1::CMXEtSums::LOCAL_RESTRICTED)  continue;
      if (remote && source != LVL1::CMXEtSums::LOCAL_STANDARD &&
                    source != LVL1::CMXEtSums::LOCAL_RESTRICTED &&
                    source != LVL1::CMXEtSums::REMOTE_STANDARD &&
                    source != LVL1::CMXEtSums::REMOTE_RESTRICTED) continue;
      if (total  && source != LVL1::CMXEtSums::TOTAL_STANDARD &&
                    source != LVL1::CMXEtSums::TOTAL_RESTRICTED)  continue;
      if (etmaps && source != LVL1::CMXEtSums::SUM_ET_STANDARD &&
                    source != LVL1::CMXEtSums::SUM_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_STANDARD &&
                    source != LVL1::CMXEtSums::MISSING_ET_RESTRICTED &&
                    source != LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD) continue;
      cmxSimEt = cmxS->Et();
      cmxSimEx = cmxS->Ex();
      cmxSimEy = cmxS->Ey();
      cmxEt    = cmxD->Et();
      cmxEx    = cmxD->Ex();
      cmxEy    = cmxD->Ey();
      if (!etmaps) {
        cmxSimEt |= ((cmxS->EtError() & 0x1) << 15);
        cmxSimEx |= ((cmxS->ExError() & 0x1) << 15);
        cmxSimEy |= ((cmxS->EyError() & 0x1) << 15);
        cmxEt    |= ((cmxD->EtError() & 0x1) << 15);
        cmxEx    |= ((cmxD->ExError() & 0x1) << 15);
        cmxEy    |= ((cmxD->EyError() & 0x1) << 15);
      }
      crate    = cmxS->crate();
    }

    if (!cmxSimEt && !cmxSimEx && !cmxSimEy && !cmxEt && !cmxEx && !cmxEy)
                                                                     continue;
    
    //  Fill in error vectors

    if (local || total || etmaps) {
      const int cmx = 0;                                                       //<< CHECK
      const int loc = crate * s_cmxs + cmx;
      const int cmxBins = s_crates * s_cmxs;
      const int bit = (local)
                        ? (1 << LocalEnergyMismatch)
                        : (total)
			    ? (1 << TotalEnergyMismatch)
			    : (source == LVL1::CMXEtSums::SUM_ET_STANDARD ||
			       source == LVL1::CMXEtSums::SUM_ET_RESTRICTED)
			       ? (1 << SumEtMismatch)
			       : (source == LVL1::CMXEtSums::MISSING_ET_STANDARD ||
			          source == LVL1::CMXEtSums::MISSING_ET_RESTRICTED)
			          ? (1 << MissingEtMismatch)
			          : (1 << MissingEtSigMismatch);
      const int offset = (source == LVL1::CMXEtSums::LOCAL_RESTRICTED  ||
                          source == LVL1::CMXEtSums::REMOTE_RESTRICTED ||
			  source == LVL1::CMXEtSums::TOTAL_RESTRICTED  ||
			  source == LVL1::CMXEtSums::SUM_ET_RESTRICTED ||
			  source == LVL1::CMXEtSums::MISSING_ET_RESTRICTED) ? 6 : 0;
      if (cmxSimEt == cmxEt && cmxSimEx == cmxEx && cmxSimEy == cmxEy) {
        errors[loc] |= bit;
      } else errors[loc+cmxBins] |= bit;
      const int loc1 = (local) ? crate : 3;
      if (local || total) {
        TH2F_LW* hist = 0;
	if (cmxSimEx && cmxSimEx == cmxEx) hist = m_h_cmx_2d_energy_SumsSimEqData;
	else if (cmxSimEx != cmxEx) {
	  if (cmxSimEx && cmxEx) hist = m_h_cmx_2d_energy_SumsSimNeData;
	  else if (!cmxEx)       hist = m_h_cmx_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 0+offset);
        hist = 0;
	if (cmxSimEy && cmxSimEy == cmxEy) hist = m_h_cmx_2d_energy_SumsSimEqData;
	else if (cmxSimEy != cmxEy) {
	  if (cmxSimEy && cmxEy) hist = m_h_cmx_2d_energy_SumsSimNeData;
	  else if (!cmxEy)       hist = m_h_cmx_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 1+offset);
        hist = 0;
	if (cmxSimEt && cmxSimEt == cmxEt) hist = m_h_cmx_2d_energy_SumsSimEqData;
	else if (cmxSimEt != cmxEt) {
	  if (cmxSimEt && cmxEt) hist = m_h_cmx_2d_energy_SumsSimNeData;
	  else if (!cmxEt)       hist = m_h_cmx_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 2+offset);
      } else {
        const int loc2 = (source == LVL1::CMXEtSums::SUM_ET_STANDARD ||
	                  source == LVL1::CMXEtSums::SUM_ET_RESTRICTED)
	                  ? 3 : (source == LVL1::CMXEtSums::MISSING_ET_STANDARD ||
			         source == LVL1::CMXEtSums::MISSING_ET_RESTRICTED)
			         ? 4 : 5;
        TH2F_LW* hist = 0;
	if (cmxSimEt && cmxSimEt == cmxEt) hist = m_h_cmx_2d_energy_SumsSimEqData;
	else if (cmxSimEt != cmxEt) {
	  if (cmxSimEt && cmxEt) hist = m_h_cmx_2d_energy_SumsSimNeData;
	  else if (!cmxEt)       hist = m_h_cmx_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, loc2+offset);
	if (cmxSimEt || cmxEt) {
	  int loc3 = 0;
	  int nThresh = 8;
	  int offset2 = 0;
	  if (source == LVL1::CMXEtSums::MISSING_ET_STANDARD ||
	      source == LVL1::CMXEtSums::MISSING_ET_RESTRICTED) {
	    loc3 = 2;
	    offset2 = 8;
          }
	  else if (source == LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD) {
	    loc3 = 4;
	    offset2 = 16;
          }
	  m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData,
	                    loc3+offset, cmxEt & cmxSimEt, nThresh, 1, offset2);
	  m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData,
	                    loc3+offset, cmxEt ^ cmxSimEt, nThresh, 1, offset2);
        }
      }
    } else {
      if (source == LVL1::CMXEtSums::LOCAL_STANDARD) {
        if (crate == 0) {
	  localEt = cmxSimEt;
	  localEx = cmxSimEx;
	  localEy = cmxSimEy;
	}
      } else if (source == LVL1::CMXEtSums::LOCAL_RESTRICTED) {
        if (crate == 0) {
	  localEtM = cmxSimEt;
	  localExM = cmxSimEx;
	  localEyM = cmxSimEy;
	}
      } else if (source == LVL1::CMXEtSums::REMOTE_STANDARD) {
        remoteEt = cmxEt;
        remoteEx = cmxEx;
        remoteEy = cmxEy;
      } else {
        remoteEtM = cmxEt;
        remoteExM = cmxEx;
        remoteEyM = cmxEy;
      }
    }
  }
  if (remote && (localEt   || localEx   || localEy  ||
                 localEtM  || localExM  || localEyM ||
                 remoteEt  || remoteEx  || remoteEy ||
		 remoteEtM || remoteExM || remoteEyM)) {
    const int crate = 1;
    const int cmx = 0;
    const int loc = crate * s_cmxs + cmx;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << RemoteEnergyMismatch);
    if (localEt == remoteEt && localEx == remoteEx && localEy == remoteEy &&
        localEtM == remoteEtM && localExM == remoteExM && localEyM == remoteEyM) {
      errors[loc] |= bit;
    } else errors[loc+cmxBins] |= bit;
    TH2F_LW* hist = 0;
    if (localEx && localEx == remoteEx) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localEx != remoteEx) {
      if (localEx && remoteEx) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteEx)      hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 0);
    hist = 0;
    if (localEy && localEy == remoteEy) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localEy != remoteEy) {
      if (localEy && remoteEy) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteEy)      hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 1);
    hist = 0;
    if (localEt && localEt == remoteEt) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localEt != remoteEt) {
      if (localEt && remoteEt) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteEt)      hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 2);
    hist = 0;
    if (localExM && localExM == remoteExM) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localExM != remoteExM) {
      if (localExM && remoteExM) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteExM)       hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 6);
    hist = 0;
    if (localEyM && localEyM == remoteEyM) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localEyM !=  remoteEyM) {
      if (localEyM && remoteEyM) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteEyM)       hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 7);
    hist = 0;
    if (localEtM && localEtM == remoteEtM) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (localEtM != remoteEtM) {
      if (localEtM && remoteEtM) hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!remoteEtM)       hist = m_h_cmx_2d_energy_SumsSimNoData;
      else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 8);
  }
}

// Compare Et Maps and Energy Totals with Energy RoIs from data

void JEPSimMon::compare(const CmxEtSumsMap& cmxMap,
                        const LVL1::CMXRoI* cmxRoi,
                              ErrorVector& errors)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Et Maps and Energy Totals with RoIs from data"
                    << endreq;
  }

  int sumEtMap = 0;
  int missEtMap = 0;
  int missEtSigMap = 0;
  int et = 0;
  int ex = 0;
  int ey = 0;
  int sumEtRoi = 0;
  int missEtRoi = 0;
  int missEtSigRoi = 0;
  int etRoi = 0;
  int exRoi = 0;
  int eyRoi = 0;
  int sumEtMapM = 0;
  int missEtMapM = 0;
  int etM = 0;
  int exM = 0;
  int eyM = 0;
  int sumEtRoiM = 0;
  int missEtRoiM = 0;
  int etRoiM = 0;
  int exRoiM = 0;
  int eyRoiM = 0;
  int key = 100 + LVL1::CMXEtSums::SUM_ET_STANDARD;
  CmxEtSumsMap::const_iterator iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    sumEtMap = sums->Et();
  }
  key = 100 + LVL1::CMXEtSums::MISSING_ET_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    missEtMap = sums->Et();
  }
  key = 100 + LVL1::CMXEtSums::MISSING_ET_SIG_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    missEtSigMap = sums->Et();
  }
  key = 100 + LVL1::CMXEtSums::TOTAL_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    et = (sums->Et() | ((sums->EtError() & 0x1) << 15));
    ex = (sums->Ex() | ((sums->ExError() & 0x1) << 15));
    ey = (sums->Ey() | ((sums->EyError() & 0x1) << 15));
  }
  key = 100 + LVL1::CMXEtSums::SUM_ET_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    sumEtMapM = sums->Et();
  }
  key = 100 + LVL1::CMXEtSums::MISSING_ET_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    missEtMapM = sums->Et();
  }
  key = 100 + LVL1::CMXEtSums::TOTAL_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const LVL1::CMXEtSums* sums = iter->second;
    etM = (sums->Et() | ((sums->EtError() & 0x1) << 15));
    exM = (sums->Ex() | ((sums->ExError() & 0x1) << 15));
    eyM = (sums->Ey() | ((sums->EyError() & 0x1) << 15));
  }
  if (cmxRoi) {
    sumEtRoi = cmxRoi->sumEtHits();
    missEtRoi = cmxRoi->missingEtHits();
    missEtSigRoi = cmxRoi->missingEtSigHits();
    etRoi = (cmxRoi->et() | ((cmxRoi->etError() & 0x1) << 15));
    exRoi = (cmxRoi->ex() | ((cmxRoi->exError() & 0x1) << 15));
    eyRoi = (cmxRoi->ey() | ((cmxRoi->eyError() & 0x1) << 15));
    sumEtRoiM = cmxRoi->sumEtHits(LVL1::CMXRoI::MASKED);
    missEtRoiM = cmxRoi->missingEtHits(LVL1::CMXRoI::MASKED);
    etRoiM = (cmxRoi->et(LVL1::CMXRoI::MASKED) | ((cmxRoi->etError() & 0x1) << 15));
    exRoiM = (cmxRoi->ex(LVL1::CMXRoI::MASKED) | ((cmxRoi->exError() & 0x1) << 15));
    eyRoiM = (cmxRoi->ey(LVL1::CMXRoI::MASKED) | ((cmxRoi->eyError() & 0x1) << 15));
  }
  if (sumEtMap || sumEtRoi || missEtMap || missEtRoi ||
      missEtSigMap || missEtSigRoi ||
      et || etRoi || ex || exRoi || ey || eyRoi ||
      sumEtMapM || sumEtRoiM || missEtMapM || missEtRoiM ||
      etM || etRoiM || exM || exRoiM || eyM || eyRoiM) {
    const int crate = 1;
    const int cmx = 0;
    const int loc = crate * s_cmxs + cmx;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << EnergyRoIMismatch);
    if (sumEtMap == sumEtRoi && missEtMap == missEtRoi &&
        missEtSigMap == missEtSigRoi &&
        et == etRoi && ex == exRoi && ey == eyRoi &&
	sumEtMapM == sumEtRoiM && missEtMapM == missEtRoiM &&
	etM == etRoiM && exM == exRoiM && eyM == eyRoiM) errors[loc] |= bit;
    else errors[loc+cmxBins] |= bit;
    TH2F_LW* hist = 0;
    if (ex && ex == exRoi)         hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (ex != exRoi) {
      if (ex && exRoi)             hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!exRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 0);
    hist = 0;
    if (ey && ey == eyRoi)         hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (ey != eyRoi) {
      if (ey && eyRoi)             hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!eyRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 1);
    hist = 0;
    if (et && et == etRoi)         hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (et != etRoi) {
      if (et && etRoi)             hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!etRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 2);
    hist = 0;
    if (sumEtMap && sumEtMap == sumEtRoi) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (sumEtMap != sumEtRoi) {
      if (sumEtMap && sumEtRoi)           hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!sumEtRoi) {
        if (!limitedRoiSet(crate))        hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                              hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 3);
    hist = 0;
    if (missEtMap && missEtMap == missEtRoi) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtMap != missEtRoi) {
      if (missEtMap && missEtRoi)            hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtRoi) {
        if (!limitedRoiSet(crate))           hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                                 hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 4);
    hist = 0;
    if (missEtSigMap && missEtSigMap == missEtSigRoi) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtSigMap != missEtSigRoi) {
      if (missEtSigMap && missEtSigRoi)               hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtSigRoi) {
        if (!limitedRoiSet(crate))                    hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                                          hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 5);
    hist = 0;
    if (exM && exM == exRoiM)      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (exM != exRoiM) {
      if (exM && exRoiM)           hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!exRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 6);
    hist = 0;
    if (eyM && eyM == eyRoiM)      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (eyM != eyRoiM) {
      if (eyM && eyRoiM)           hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!eyRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 7);
    hist = 0;
    if (etM && etM == etRoiM)      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (etM != etRoiM) {
      if (etM && etRoiM)           hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!etRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 8);
    hist = 0;
    if (sumEtMapM && sumEtMapM == sumEtRoiM) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (sumEtMapM != sumEtRoiM) {
      if (sumEtMapM && sumEtRoiM)            hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!sumEtRoiM) {
        if (!limitedRoiSet(crate))           hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                                 hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 9);
    hist = 0;
    if (missEtMapM && missEtMapM == missEtRoiM) hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtMapM != missEtRoiM) {
      if (missEtMapM && missEtRoiM)             hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtRoiM) {
        if (!limitedRoiSet(crate))              hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else                                    hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 10);

    const int thrLen = 1;
    const int nThresh = 8;
    if ((sumEtMap || sumEtRoi) && !(sumEtMap && !sumEtRoi 
                               && limitedRoiSet(crate))) {
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 1,
                                    sumEtRoi & sumEtMap, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 1,
                                    sumEtRoi ^ sumEtMap, nThresh, thrLen);
    }
    if ((missEtMap || missEtRoi) && !(missEtMap && !missEtRoi
                                 && limitedRoiSet(crate))) {
      const int offset  = 8;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 3,
                              missEtRoi & missEtMap, nThresh, thrLen, offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 3,
                              missEtRoi ^ missEtMap, nThresh, thrLen, offset);
    }
    if ((missEtSigMap || missEtSigRoi) && !(missEtSigMap && !missEtSigRoi
                                       && limitedRoiSet(crate))) {
      const int offset  = 16;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 5,
                         missEtSigRoi & missEtSigMap, nThresh, thrLen, offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 5,
                         missEtSigRoi ^ missEtSigMap, nThresh, thrLen, offset);
    }
    if ((sumEtMapM || sumEtRoiM) && !(sumEtMapM && !sumEtRoiM 
                                 && limitedRoiSet(crate))) {
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 7,
                                    sumEtRoiM & sumEtMapM, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 7,
                                    sumEtRoiM ^ sumEtMapM, nThresh, thrLen);
    }
    if ((missEtMapM || missEtRoiM) && !(missEtMapM && !missEtRoiM
                                   && limitedRoiSet(crate))) {
      const int offset  = 8;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 9,
                              missEtRoiM & missEtMapM, nThresh, thrLen, offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 9,
                              missEtRoiM ^ missEtMapM, nThresh, thrLen, offset);
    }
  }
}

void JEPSimMon::fillEventSample(int err, int loc, bool isJem)
{
  int hist = 0;
  int y    = 0;
  if (isJem) {
    hist = (err < 4) ? err : err - 4;
    y    = loc;
  } else {
    hist = (err < 8) ? 6 : 7;
    y = loc/s_cmxs; // crate number
    if      (err == RemoteJetMismatch)    y = 2; // first 2 Local 0/1
    else if (err == TotalJetMismatch)     y = 3;
    else if (err == CMXJetTopoMismatch)   y += 4;
    else if (err == LocalEnergyMismatch)  y += 2;// first 2 Module 0/1
    else if (err == RemoteEnergyMismatch) y = 4;
    else if (err == TotalEnergyMismatch)  y = 5;
    else if (err == SumEtMismatch)        y = 6;
    else if (err == MissingEtMismatch)    y = 7;
    else if (err == MissingEtSigMismatch) y = 8;
    else if (err == EnergyRoIMismatch)    y = 9;
  }
  if (m_v_2d_MismatchEvents[hist]) m_histTool->fillEventNumber(m_v_2d_MismatchEvents[hist], y);
}

void JEPSimMon::setLabels(LWHist* hist, bool xAxis)
{
  // Simulation steps in red (#color[2]) depend on Trigger Menu
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EMElementMismatch,    "EM je");
  axis->SetBinLabel(1+HadElementMismatch,   "Had je");
  axis->SetBinLabel(1+RoIMismatch,          "#color[2]{RoIs}");
  axis->SetBinLabel(1+CMXJetTobMismatch,    "CMXTob");
  axis->SetBinLabel(1+LocalJetMismatch,     "#color[2]{Local}");
  axis->SetBinLabel(1+RemoteJetMismatch,    "Remote");
  axis->SetBinLabel(1+TotalJetMismatch,     "Total");
  axis->SetBinLabel(1+CMXJetTopoMismatch,   "Topo");
  axis->SetBinLabel(1+JEMEtSumsMismatch,    "JEMSums");
  axis->SetBinLabel(1+CMXEtSumsMismatch,    "CMXSums");
  axis->SetBinLabel(1+LocalEnergyMismatch,  "#color[2]{Local}");
  axis->SetBinLabel(1+RemoteEnergyMismatch, "Remote");
  axis->SetBinLabel(1+TotalEnergyMismatch,  "Total");
  axis->SetBinLabel(1+SumEtMismatch,        "#color[2]{SumEt}");
  axis->SetBinLabel(1+MissingEtMismatch,    "#color[2]{MissingEt}");
  axis->SetBinLabel(1+MissingEtSigMismatch, "#color[2]{MissEtSig}");
  axis->SetBinLabel(1+EnergyRoIMismatch,    "Engy RoIs");
}

void JEPSimMon::setLabelsSH(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "Local0");
  axis->SetBinLabel(2, "Local1");
  axis->SetBinLabel(3, "Remote");
  axis->SetBinLabel(4, "Total");
}

void JEPSimMon::setLabelsSHF(LWHist* hist)
{
  setLabelsSH(hist);
  m_histTool->jemThresholds(hist, 0, false);
}

void JEPSimMon::setLabelsTopo(TH2F_LW* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "Crate 0");
  axis->SetBinLabel(2, "Crate 1");
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Checksum");
  axis->SetBinLabel(2, "Map");
  axis->SetBinLabel(3, "Counts");
  axis->SetTitle("Topo Info");
}

void JEPSimMon::setLabelsEnTot(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "Local0");
  axis->SetBinLabel(2, "Local1");
  axis->SetBinLabel(3, "Remote");
  axis->SetBinLabel(4, "Total");
  axis->SetBinLabel(5, "RoI");
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Ex");
  axis->SetBinLabel(2, "Ey");
  axis->SetBinLabel(3, "Et");
  axis->SetBinLabel(4, "SumEt");
  axis->SetBinLabel(5, "MissingEt");
  axis->SetBinLabel(6, "MissEtSig");
  axis->SetBinLabel(7, "ExM");
  axis->SetBinLabel(8, "EyM");
  axis->SetBinLabel(9, "EtM");
  axis->SetBinLabel(10, "SumEtM");
  axis->SetBinLabel(11, "MissingEtM");
}

void JEPSimMon::setLabelsEnTotThr(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "SumEt");
  axis->SetBinLabel(2, "SumEt RoI");
  axis->SetBinLabel(3, "MissingEt");
  axis->SetBinLabel(4, "MissingEt RoI");
  axis->SetBinLabel(5, "MissingEtSig");
  axis->SetBinLabel(6, "MissingEtSig RoI");
  axis->SetBinLabel(7, "SumEtM");
  axis->SetBinLabel(8, "SumEtM RoI");
  axis->SetBinLabel(9, "MissingEtM");
  axis->SetBinLabel(10, "MissingEtM RoI");
  m_histTool->sumEtThresholds(hist, 0, false);
  m_histTool->missingEtThresholds(hist, 8, false);
  m_histTool->missingEtSigThresholds(hist, 16, false);
}

void JEPSimMon::setupMap(const JetElementCollection* coll,
                               JetElementMap& map)
{
  if (coll) m_jetElementTool->mapJetElements(coll, &map);
}

void JEPSimMon::setupMap(const JemRoiCollection* coll, JemRoiMap& map)
{
  if (coll) {
    JemRoiCollection::const_iterator pos  = coll->begin();
    JemRoiCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->jem();
      const int frame = (*pos)->frame();
      const int loc   = (*pos)->location();
      const int key   = ((((((crate << 4) | jem) << 3) | frame) << 3) | loc);
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimMon::setupMap(const CmxJetTobCollection* coll, CmxJetTobMap& map)
{
  if (coll) {
    CmxJetTobCollection::const_iterator pos  = coll->begin();
    CmxJetTobCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->jem();
      const int frame = (*pos)->frame();
      const int loc   = (*pos)->location();
      const int key   = ((((((crate << 4) | jem) << 3) | frame) << 3) | loc);
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimMon::setupMap(const CmxJetHitsCollection* coll, CmxJetHitsMap& map)
{
  if (coll) {
    CmxJetHitsCollection::const_iterator pos  = coll->begin();
    CmxJetHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int source = (*pos)->source();
      const int key  = crate * 100 + source;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimMon::setupMap(const JemEtSumsCollection* coll, JemEtSumsMap& map)
{
  if (coll) {
    JemEtSumsCollection::const_iterator pos  = coll->begin();
    JemEtSumsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->module();
      const int key   = crate * 100 + jem;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimMon::setupMap(const CmxEtSumsCollection* coll, CmxEtSumsMap& map)
{
  if (coll) {
    CmxEtSumsCollection::const_iterator pos  = coll->begin();
    CmxEtSumsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int source = (*pos)->source();
      const int key  = crate * 100 + source;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimMon::simulate(const TriggerTowerCollection* towers,
                               JetElementCollection* elements)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate Jet Elements from Trigger Towers" << endreq;
  }

  // Make zero-suppressed collection to speed up simulation

  TriggerTowerCollection* towersZ =
                              new TriggerTowerCollection(SG::VIEW_ELEMENTS);
  TriggerTowerCollection::const_iterator pos  = towers->begin();
  TriggerTowerCollection::const_iterator posE = towers->end();
  for (; pos != posE; ++pos) {
    if ((*pos)->emEnergy() > 0 || (*pos)->hadEnergy() > 0) {
      towersZ->push_back(*pos);
    }
  }
  m_jetElementTool->makeJetElements(towersZ, elements);
  delete towersZ;
}

void JEPSimMon::simulate(const JetElementCollection* elements,
                         const JetElementCollection* elementsOv,
                               JemRoiCollection* rois)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate JEM RoIs from Jet Elements" << endreq;
  }

  // Process a crate at a time to use overlap data
  const int ncrates = 2;
  std::vector<JetElementCollection*> crateColl;
  for (int crate = 0; crate < ncrates; ++crate) {
    crateColl.push_back(new JetElementCollection(SG::VIEW_ELEMENTS));
  }
  LVL1::CoordToHardware converter;
  JetElementCollection::const_iterator iter;
  JetElementCollection::const_iterator iterE;
  if (elements) {  // core data
    iter  = elements->begin();
    iterE = elements->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrate(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  }
  if (elementsOv) {  // overlap data
    iter  = elementsOv->begin();
    iterE = elementsOv->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrateOverlap(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  } else if (elements) {  // take overlap from core
    iter  = elements->begin();
    iterE = elements->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrateOverlap(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  }
  for (int crate = 0; crate < ncrates; ++crate) {
    InternalRoiCollection* intRois = new InternalRoiCollection;
    m_jetTool->findRoIs(crateColl[crate], intRois);
    JemRoiCollection* roiTemp = new JemRoiCollection(SG::VIEW_ELEMENTS);
    m_jetCmxTool->formJEMTobRoI(intRois, roiTemp);
    JemRoiCollection::iterator roiIter  = roiTemp->begin();
    JemRoiCollection::iterator roiIterE = roiTemp->end();
    for (; roiIter != roiIterE; ++roiIter) {
      if ((*roiIter)->crate() == crate) {
        rois->push_back(*roiIter);
      }
    }
    delete intRois;
    delete roiTemp;
    delete crateColl[crate];
  }
}

// Quicker version when core and overlap the same

void JEPSimMon::simulate(const JetElementCollection* elements,
                               JemRoiCollection* rois)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate JEM RoIs from Jet Elements" << endreq;
  }

  InternalRoiCollection* intRois = new InternalRoiCollection;
  if(elements) {
    m_jetTool->findRoIs(elements, intRois);
    m_jetCmxTool->formJEMTobRoI(intRois, rois);
  }
  delete intRois;
}

void JEPSimMon::simulate(const JemRoiCollection* rois,
                               CmxJetTobCollection* tobs)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX-Jet TOBs from JEM RoIs"
                               << endreq;
  m_jetCmxTool->formCMXJetTob(rois, tobs);
}

void JEPSimMon::simulate(const CmxJetTobCollection* tobs,
                               CmxJetHitsCollection* hits,
			       int selection)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX-Jet Hit sums from CMX-Jet TOBs"
                               << endreq;

  if (selection == LVL1::CMXJetHits::LOCAL_MAIN) {
    m_jetCmxTool->formCMXJetHitsCrate(tobs, hits);
  } else if (selection == LVL1::CMXJetHits::TOPO_CHECKSUM) {
    m_jetCmxTool->formCMXJetHitsTopo(tobs, hits);
  }
}

void JEPSimMon::simulate(const CmxJetHitsCollection* hitsIn,
                               CmxJetHitsCollection* hitsOut)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMX Total Hit sums from Remote/Local"
                               << endreq;

  m_jetCmxTool->formCMXJetHitsSystem(hitsIn, hitsOut);
}

void JEPSimMon::simulate(const JetElementCollection* elements,
                               JemEtSumsCollection* sums)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate JEM EtSums from JetElements"
                               << endreq;

  m_energyCmxTool->formJEMEtSums(elements, sums);
}

void JEPSimMon::simulate(const CmxEtSumsCollection* sumsIn,
                               CmxEtSumsCollection* sumsOut,
			       int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate CMX-Energy Total sums from CMX-Energy Sums"
                    << endreq;
  }

  if (selection == LVL1::CMXEtSums::LOCAL_STANDARD) {
    m_energyCmxTool->formCMXEtSumsCrate(sumsIn, sumsOut);
  } else if (selection == LVL1::CMXEtSums::TOTAL_STANDARD) {
    m_energyCmxTool->formCMXEtSumsSystem(sumsIn, sumsOut);
  } else if (selection == LVL1::CMXEtSums::SUM_ET_STANDARD) {
    m_energyCmxTool->formCMXEtSumsEtMaps(sumsIn, sumsOut);
  }
}

// Check if LimitedRoISet bit set

bool JEPSimMon::limitedRoiSet(int crate)
{
  if (m_rodTES) loadRodHeaders();
  return (((m_limitedRoi>>crate)&0x1) == 1);
}

// Load ROD Headers

void JEPSimMon::loadRodHeaders()
{
  if (m_rodTES) {
    m_limitedRoi = 0;
    RodHeaderCollection::const_iterator rodIter  = m_rodTES->begin();
    RodHeaderCollection::const_iterator rodIterE = m_rodTES->end();
    for (; rodIter != rodIterE; ++rodIter) {
      const LVL1::RODHeader* rod = *rodIter;
      const int rodCrate = rod->crate() - 12;
      if (rodCrate >= 0 && rodCrate < s_crates && rod->dataType() == 1) {
        if (rod->limitedRoISet()) {
          m_limitedRoi |= (1<<rodCrate);
        }
      }
    }
    m_rodTES = 0;
  }
}
// ============================================================================
} // end namespace
// ============================================================================