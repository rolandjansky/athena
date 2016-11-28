/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloBSMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"
#include "AthContainers/DataVector.h"

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloMonitoringCaloTool.h"

#include "TrigT1CaloBSMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
TrigT1CaloBSMon::TrigT1CaloBSMon(const std::string & type, 
			 const std::string & name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_caloTool("LVL1::L1CaloMonitoringCaloTool/L1CaloMonitoringCaloTool")
/*---------------------------------------------------------*/
{
  declareProperty("LoadL1Calo",    m_l1calo = false);
  declareProperty("LoadL1CaloPPM", m_l1caloPPM = false);
  declareProperty("LoadL1CaloCPM", m_l1caloCPM = false);
  declareProperty("LoadL1CaloJEM", m_l1caloJEM = false);
  declareProperty("LoadL1CaloROD", m_l1caloROD = false);
  declareProperty("LoadCaloCells", m_caloCells = false);

  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		                 LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("CPMTowerLocation",
                 m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                 m_cpmTowerLocationOverlap  =
                           LVL1::TrigT1CaloDefs::CPMTowerLocation+"Overlap");
  declareProperty("CPMHitsLocation",
                 m_cpmHitsLocation   = LVL1::TrigT1CaloDefs::CPMHitsLocation);
  declareProperty("CMMCPHitsLocation",
                 m_cmmCpHitsLocation = LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
  declareProperty("CPMRoILocation",
                 m_cpmRoiLocation    = LVL1::TrigT1CaloDefs::CPMRoILocation);
  declareProperty("JetElementLocation",
                 m_jetElementLocation =
                                 LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JetElementLocationOverlap",
                 m_jetElementLocationOverlap =
                           LVL1::TrigT1CaloDefs::JetElementLocation+"Overlap");
  declareProperty("JEMHitsLocation",
                 m_jemHitsLocation    = LVL1::TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("CMMJetHitsLocation",
                 m_cmmJetHitsLocation =
                                 LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
  declareProperty("JEMRoILocation",
                 m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMRoILocation);
  declareProperty("CMMRoILocation",
                 m_cmmRoiLocation     = LVL1::TrigT1CaloDefs::CMMRoILocation);
  declareProperty("JEMEtSumsLocation",
                 m_jemEtSumsLocation = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMMEtSumsLocation",
                 m_cmmEtSumsLocation = LVL1::TrigT1CaloDefs::CMMEtSumsLocation);
  declareProperty("RodHeaderLocation", m_rodHeaderLocation = "RODHeaders");
  m_cpRoibRodHeaderLocation  = m_rodHeaderLocation + "CPRoIB";
  m_jepRoibRodHeaderLocation = m_rodHeaderLocation + "JEPRoIB";
  declareProperty("L1CaloErrorLocation",
                   m_robErrorVectorLocation = "L1CaloUnpackingErrors");
}

/*---------------------------------------------------------*/
TrigT1CaloBSMon::~TrigT1CaloBSMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode TrigT1CaloBSMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  if (m_caloCells) {
    sc = m_caloTool.retrieve();
    if( sc.isFailure() ) {
      msg(MSG::ERROR) << "Unable to locate Tool L1CaloMonitoringCaloTool"
                      << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloBSMon::fillHistograms()
/*---------------------------------------------------------*/
{
  
  // Load L1Calo data

  typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;
  typedef DataVector<LVL1::CPMTower>     CpmTowerCollection;
  typedef DataVector<LVL1::CPMHits>      CpmHitsCollection;
  typedef DataVector<LVL1::CMMCPHits>    CmmCpHitsCollection;
  typedef DataVector<LVL1::CPMRoI>       CpmRoiCollection;
  typedef DataVector<LVL1::JetElement>   JetElementCollection;
  typedef DataVector<LVL1::JEMHits>      JemHitsCollection;
  typedef DataVector<LVL1::CMMJetHits>   CmmJetHitsCollection;
  typedef DataVector<LVL1::JEMRoI>       JemRoiCollection;
  typedef DataVector<LVL1::JEMEtSums>    JemEtSumsCollection;
  typedef DataVector<LVL1::CMMEtSums>    CmmEtSumsCollection;
  typedef DataVector<LVL1::RODHeader>    RodHeaderCollection;

  if (m_l1calo || m_l1caloPPM) {

    StatusCode sc;

    //Retrieve Trigger Towers from SG
    const TriggerTowerCollection* triggerTowerTES = 0; 
    const TriggerTowerCollection* triggerTowerSpareTES = 0; 
    const TriggerTowerCollection* triggerTowerMuonTES = 0; 
    sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
    if( sc.isFailure()  ||  !triggerTowerTES ) {
      msg(MSG::WARNING) << "No Trigger Tower container found"<< endmsg; 
    }
    sc = evtStore()->retrieve(triggerTowerSpareTES,
                                         m_triggerTowerLocation+"Spare"); 
    if( sc.isFailure()  ||  !triggerTowerSpareTES ) {
      msg(MSG::WARNING) << "No Spare Trigger Tower container found"<< endmsg; 
    }
    sc = evtStore()->retrieve(triggerTowerMuonTES,
                                         m_triggerTowerLocation+"Muon"); 
    if( sc.isFailure()  ||  !triggerTowerMuonTES ) {
      msg(MSG::WARNING) << "No Tile Muon Trigger Tower container found"<< endmsg; 
    }
  }

  if (m_l1calo || m_l1caloCPM) {

    StatusCode sc;

    //Retrieve Core and Overlap CPM Towers from SG
    const CpmTowerCollection* cpmTowerTES = 0; 
    const CpmTowerCollection* cpmTowerOvTES = 0; 
    sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation); 
    if( sc.isFailure()  ||  !cpmTowerTES ) {
      msg(MSG::WARNING) << "No Core CPM Tower container found"<< endmsg; 
    }
    sc = evtStore()->retrieve(cpmTowerOvTES, m_cpmTowerLocationOverlap); 
    if( sc.isFailure()  ||  !cpmTowerOvTES ) {
      msg(MSG::WARNING) << "No Overlap CPM Tower container found"<< endmsg; 
    }
  
    //Retrieve CPM RoIs from SG
    const CpmRoiCollection* cpmRoiTES = 0;
    sc = evtStore()->retrieve( cpmRoiTES, m_cpmRoiLocation);
    if( sc.isFailure()  ||  !cpmRoiTES ) {
      msg(MSG::WARNING) << "No CPM RoIs container found"<< endmsg;
    }
  
    //Retrieve CPM Hits from SG
    const CpmHitsCollection* cpmHitsTES = 0;
    sc = evtStore()->retrieve( cpmHitsTES, m_cpmHitsLocation);
    if( sc.isFailure()  ||  !cpmHitsTES ) {
      msg(MSG::WARNING) << "No CPM Hits container found"<< endmsg; 
    }
  
    //Retrieve CMM-CP Hits from SG
    const CmmCpHitsCollection* cmmCpHitsTES = 0;
    sc = evtStore()->retrieve( cmmCpHitsTES, m_cmmCpHitsLocation);
    if( sc.isFailure()  ||  !cmmCpHitsTES ) {
      msg(MSG::WARNING) << "No CMM-CP Hits container found"<< endmsg; 
    }

  }

  if (m_l1calo || m_l1caloJEM) {

    StatusCode sc;

    //Retrieve Core and Overlap Jet Elements from SG
    const JetElementCollection* jetElementTES = 0; 
    const JetElementCollection* jetElementOvTES = 0; 
    sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation); 
    if( sc.isFailure()  ||  !jetElementTES ) {
      msg(MSG::WARNING) << "No Core Jet Element container found"<< endmsg; 
    }
    sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
    if( sc.isFailure()  ||  !jetElementOvTES ) {
      msg(MSG::WARNING) << "No Overlap Jet Element container found"<< endmsg;
    }
  
    //Retrieve JEM RoIs from SG
    const JemRoiCollection* jemRoiTES = 0;
    sc = evtStore()->retrieve( jemRoiTES, m_jemRoiLocation);
    if( sc.isFailure()  ||  !jemRoiTES  ) {
      msg(MSG::WARNING) << "No DAQ JEM RoIs container found" << endmsg; 
    }
  
    //Retrieve JEM Hits from SG
    const JemHitsCollection* jemHitsTES = 0;
    sc = evtStore()->retrieve( jemHitsTES, m_jemHitsLocation);
    if( sc.isFailure()  ||  !jemHitsTES ) {
      msg(MSG::WARNING) << "No JEM Hits container found"<< endmsg; 
    }
  
    //Retrieve CMM-Jet Hits from SG
    const CmmJetHitsCollection* cmmJetHitsTES = 0;
    sc = evtStore()->retrieve( cmmJetHitsTES, m_cmmJetHitsLocation);
    if( sc.isFailure()  ||  !cmmJetHitsTES ) {
      msg(MSG::WARNING) << "No CMM-Jet Hits container found"<< endmsg; 
    }
  
    //Retrieve CMM RoIs from SG
    const LVL1::CMMRoI* cmmRoiTES = 0;
    sc = evtStore()->retrieve( cmmRoiTES, m_cmmRoiLocation);
    if( sc.isFailure()  ||  !cmmRoiTES ) {
      msg(MSG::WARNING) << "No CMM RoIs container found" << endmsg; 
    }

    //Retrieve JEM Et Sums from SG
    const JemEtSumsCollection* jemEtSumsTES = 0;
    sc = evtStore()->retrieve( jemEtSumsTES, m_jemEtSumsLocation);
    if( sc.isFailure()  ||  !jemEtSumsTES ) {
      msg(MSG::WARNING) << "No JEM Et Sums container found"<< endmsg;
    }

    //Retrieve CMM Et Sums from SG
    const CmmEtSumsCollection* cmmEtSumsTES = 0;
    sc = evtStore()->retrieve( cmmEtSumsTES, m_cmmEtSumsLocation);
    if( sc.isFailure()  ||  !cmmEtSumsTES ) {
      msg(MSG::WARNING) << "No CMM-Energy Et Sums container found"<< endmsg;
    }

  }

  if (m_l1calo || m_l1caloROD) {

    StatusCode sc;

    //Retrieve ROD Headers from SG
    const RodHeaderCollection* rodTES = 0; 
    sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation); 
    if( sc.isFailure()  ||  !rodTES ) {
      msg(MSG::WARNING) << "No ROD Header container found"<< endmsg; 
    }

    //Retrieve CP RoIB ROD Headers from SG
    const RodHeaderCollection* cpRoibTES = 0; 
    sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation); 
    if( sc.isFailure()  ||  !cpRoibTES ) {
      msg(MSG::WARNING) << "No CP RoIB ROD Header container found"<< endmsg; 
    }

    //Retrieve JEP RoIB ROD Headers from SG
    const RodHeaderCollection* jepRoibTES = 0; 
    sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation); 
    if( sc.isFailure()  ||  !jepRoibTES ) {
      msg(MSG::WARNING) << "No JEP RoIB ROD Header container found"<< endmsg; 
    }

    //Retrieve ROB Status and Unpacking Error vector from SG
    const std::vector<unsigned int>* errColl = 0;
    sc = evtStore()->retrieve(errColl, m_robErrorVectorLocation);
    if( sc.isFailure()  ||  !errColl ) {
      msg(MSG::WARNING) << "No ROB Status and Unpacking Error vector found"
                        << endmsg;
    }
  }

  // Load CaloCells for faster data retrieval

  if (m_caloCells) {
    StatusCode sc = m_caloTool->loadCaloCells();
    if (sc.isFailure()) return sc;
  }

  return StatusCode::SUCCESS;
}

// ============================================================================
}  // end namespace
// ============================================================================
