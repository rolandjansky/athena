/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"
#include "AthContainers/DataVector.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
// #include "xAODTrigL1Calo/CPMRoIContainer.h"
// #include "xAODTrigL1Calo/JEMRoIContainer.h"


#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"


#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloMonErrorTool.h"

namespace LVL1 {


// Constructor

TrigT1CaloMonErrorTool::TrigT1CaloMonErrorTool(
  const std::string& name)
  : AsgTool(name)
{

  declareProperty("TriggerTowerLocation",
                  m_triggerTowerLocation =
                    LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("CPMTowerLocation",
                  m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                  m_cpmTowerLocationOverlap  =
                    LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap");
  declareProperty("CMXCPTobLocation",
                  m_cmxCpTobLocation   = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
  declareProperty("CMXCPHitsLocation",
                  m_cmxCpHitsLocation  = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);
  declareProperty("CPMTobRoILocation",
                  m_cpmRoiLocation     = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("JetElementLocation",
                  m_jetElementLocation = LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JetElementLocationOverlap",
                  m_jetElementLocationOverlap =
                    LVL1::TrigT1CaloDefs::JetElementLocation + "Overlap");
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
  declareProperty("RodHeaderLocation",
                  m_rodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation);
  declareProperty("RodHeaderLocationCPRoIB",
                  m_cpRoibRodHeaderLocation  = LVL1::TrigT1CaloDefs::RODHeaderLocation + "CPRoIB");
  declareProperty("RodHeaderLocationJEPRoIB",
                  m_jepRoibRodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation + "JEPRoIB");
  declareProperty("L1CaloErrorLocation",
                  m_robErrorVectorLocation = "L1CaloUnpackingErrors");
  declareProperty("FlagCorruptEvents", m_flagCorruptEvents = "FullEventTimeout",
                  "Can be FullEventTimeout, AnyROBOrUnpackingError or None");

}

// Destructor

TrigT1CaloMonErrorTool::~TrigT1CaloMonErrorTool()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode TrigT1CaloMonErrorTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode TrigT1CaloMonErrorTool::finalize()
{
  return StatusCode::SUCCESS;
}

// Retrieve error vector

StatusCode TrigT1CaloMonErrorTool::retrieve(const std::vector<unsigned int>*&
    errColl)
{

  // Must ensure bytestream converters have unpacked all our data
  // before retrieving error vector.

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const xAOD::TriggerTowerContainer* triggerTowerTES = 0;
  const xAOD::TriggerTowerContainer* triggerTowerSpareTES = 0;
  const xAOD::TriggerTowerContainer* triggerTowerMuonTES = 0;
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation);
  if ( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endmsg;
  }
  if (evtStore()->contains<xAOD::TriggerTowerContainer>(m_triggerTowerLocation + "Spare")) {
    sc = evtStore()->retrieve(triggerTowerSpareTES,
                              m_triggerTowerLocation + "Spare");
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !triggerTowerSpareTES ) {
    msg(MSG::DEBUG) << "No Spare Trigger Tower container found" << endmsg;
  }
  if (evtStore()->contains<xAOD::TriggerTowerContainer>(m_triggerTowerLocation + "Muon")) {
    sc = evtStore()->retrieve(triggerTowerMuonTES,
                              m_triggerTowerLocation + "Muon");
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !triggerTowerMuonTES ) {
    msg(MSG::DEBUG) << "No Tile Muon Trigger Tower container found" << endmsg;
  }

  //Retrieve Core and Overlap CPM Towers from SG
  const xAOD::CPMTowerContainer* cpmTowerTES = 0;
  const xAOD::CPMTowerContainer* cpmTowerOvTES = 0;
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation);
  if ( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found" << endmsg;
  }
  if (evtStore()->contains<xAOD::CPMTowerContainer>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOvTES, m_cpmTowerLocationOverlap);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpmTowerOvTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found" << endmsg;
  }

  //Retrieve CPM RoIs from SG
  const xAOD::CPMTobRoIContainer* cpmRoiTES = 0;
  sc = evtStore()->retrieve( cpmRoiTES, m_cpmRoiLocation);
  if ( sc.isFailure()  ||  !cpmRoiTES ) {
    msg(MSG::DEBUG) << "No CPM RoIs container found" << endmsg;
  }

  //Retrieve CMX-CP TOBs from SG
  const xAOD::CMXCPTobContainer* cmxCpTobTES = 0;
  sc = evtStore()->retrieve( cmxCpTobTES, m_cmxCpTobLocation);
  if ( sc.isFailure()  ||  !cmxCpTobTES ) {
    msg(MSG::DEBUG) << "No CMX-CP TOB container found" << endmsg;
  }

  //Retrieve CMX-CP Hits from SG
  const xAOD::CMXCPHitsContainer* cmxCpHitsTES = 0;
  sc = evtStore()->retrieve( cmxCpHitsTES, m_cmxCpHitsLocation);
  if ( sc.isFailure()  ||  !cmxCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMX-CP Hits container found" << endmsg;
  }

  //Retrieve Core and Overlap Jet Elements from SG
  const xAOD::JetElementContainer* jetElementTES = 0;
  const xAOD::JetElementContainer* jetElementOvTES = 0;
  sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation);
  if ( sc.isFailure()  ||  !jetElementTES ) {
    msg(MSG::DEBUG) << "No Core Jet Element container found" << endmsg;
  }
  if (evtStore()->contains<xAOD::JetElementContainer>(m_jetElementLocationOverlap)) {
    sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jetElementOvTES ) {
    msg(MSG::DEBUG) << "No Overlap Jet Element container found" << endmsg;
  }

  //Retrieve JEM RoIs from SG
  const xAOD::JEMTobRoIContainer* jemRoiTES = 0;
  sc = evtStore()->retrieve( jemRoiTES, m_jemRoiLocation);
  if ( sc.isFailure()  ||  !jemRoiTES  ) {
    msg(MSG::DEBUG) << "No DAQ JEM RoIs container found" << endmsg;
  }

  //Retrieve CMX-Jet TOBs from SG
  const xAOD::CMXJetTobContainer* cmxJetTobTES = 0;
  sc = evtStore()->retrieve( cmxJetTobTES, m_cmxJetTobLocation);
  if ( sc.isFailure()  ||  !cmxJetTobTES ) {
    msg(MSG::DEBUG) << "No CMX-Jet TOB container found" << endmsg;
  }

  //Retrieve CMX-Jet Hits from SG
  const xAOD::CMXJetHitsContainer* cmxJetHitsTES = 0;
  sc = evtStore()->retrieve( cmxJetHitsTES, m_cmxJetHitsLocation);
  if ( sc.isFailure()  ||  !cmxJetHitsTES ) {
    msg(MSG::DEBUG) << "No CMX-Jet Hits container found" << endmsg;
  }

  //Retrieve CMX RoIs from SG
  const xAOD::CMXRoIContainer* cmxRoiTES = 0;
  sc = evtStore()->retrieve( cmxRoiTES, m_cmxRoiLocation);
  if ( sc.isFailure()  ||  !cmxRoiTES ) {
    msg(MSG::DEBUG) << "No CMX RoIs container found" << endmsg;
  }

  //Retrieve JEM Et Sums from SG
  const xAOD::JEMEtSumsContainer* jemEtSumsTES = 0;
  sc = evtStore()->retrieve( jemEtSumsTES, m_jemEtSumsLocation);
  if ( sc.isFailure()  ||  !jemEtSumsTES ) {
    msg(MSG::DEBUG) << "No JEM Et Sums container found" << endmsg;
  }

  //Retrieve CMX Et Sums from SG
  const xAOD::CMXEtSumsContainer* cmxEtSumsTES = 0;
  sc = evtStore()->retrieve( cmxEtSumsTES, m_cmxEtSumsLocation);
  if ( sc.isFailure()  ||  !cmxEtSumsTES ) {
    msg(MSG::DEBUG) << "No CMX-Energy Et Sums container found" << endmsg;
  }

  //Retrieve ROD Headers from SG
  const xAOD::RODHeaderContainer* rodTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endmsg;
  }

  //Retrieve CP RoIB ROD Headers from SG
  const xAOD::RODHeaderContainer* cpRoibTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_cpRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpRoibTES ) {
    msg(MSG::DEBUG) << "No CP RoIB ROD Header container found" << endmsg;
  }

  //Retrieve JEP RoIB ROD Headers from SG
  const xAOD::RODHeaderContainer* jepRoibTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_jepRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jepRoibTES ) {
    msg(MSG::DEBUG) << "No JEP RoIB ROD Header container found" << endmsg;
  }

  //Retrieve ROB Status and Unpacking Error vector from SG
  errColl = 0;
  if (evtStore()->contains<std::vector<unsigned int> >(m_robErrorVectorLocation)) {
    sc = evtStore()->retrieve(errColl, m_robErrorVectorLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !errColl ) {
    msg(MSG::DEBUG) << "No ROB Status and Unpacking Error vector found"
                    << endmsg;
  }

  return sc;
}

// Return true if current event has any corruption errors

bool TrigT1CaloMonErrorTool::corrupt()
{
  if (m_flagCorruptEvents == "AnyROBOrUnpackingError") {
    return robOrUnpackingError();
  } else if (m_flagCorruptEvents == "FullEventTimeout") {
    return (fullEventTimeout() && missingFragment() && !robOrUnpackingError());
  }

  return false;
}

// Return true if current event has Full Event status generic timeout bit set

bool TrigT1CaloMonErrorTool::fullEventTimeout()
{
  unsigned int evtStatus = 0;
  const EventInfo* evtInfo = 0;
  StatusCode sc = evtStore()->retrieve(evtInfo);
  if ( sc.isSuccess() ) {
    const TriggerInfo* trigInfo = evtInfo->trigger_info();
    if (trigInfo) evtStatus = trigInfo->statusElement();
  }
  return (((evtStatus >> 2) & 0x1) == 1);
}

// Return true if any ROB/ROD fragments are missing

bool TrigT1CaloMonErrorTool::missingFragment()
{

  const bool debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  //Retrieve DAQ ROD Headers from SG
  const xAOD::RODHeaderContainer* rodTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !rodTES ) {
    if (debug) msg(MSG::DEBUG) << "No DAQ ROD Header container found"
                                 << endmsg;
  }

  //Retrieve CP RoIB ROD Headers from SG
  const xAOD::RODHeaderContainer* cpRoibTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_cpRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpRoibTES ) {
    if (debug) msg(MSG::DEBUG) << "No CP RoIB ROD Header container found"
                                 << endmsg;
  }

  //Retrieve JEP RoIB ROD Headers from SG
  const xAOD::RODHeaderContainer* jepRoibTES = 0;
  if (evtStore()->contains<xAOD::RODHeaderContainer>(m_jepRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jepRoibTES ) {
    if (debug) msg(MSG::DEBUG) << "No JEP RoIB ROD Header container found"
                                 << endmsg;
  }

  // Record fragments present
  std::vector<int> noFragmentFlags(80, 1);
  std::vector<const xAOD::RODHeaderContainer*> cols;
  if (rodTES)     cols.push_back(rodTES);
  if (cpRoibTES)  cols.push_back(cpRoibTES);
  if (jepRoibTES) cols.push_back(jepRoibTES);
  std::vector<const xAOD::RODHeaderContainer*>::const_iterator colIter =
    cols.begin();
  std::vector<const xAOD::RODHeaderContainer*>::const_iterator colIterEnd =
    cols.end();
  for (; colIter != colIterEnd; ++colIter) {
    xAOD::RODHeaderContainer::const_iterator iter    = (*colIter)->begin();
    xAOD::RODHeaderContainer::const_iterator iterEnd = (*colIter)->end();
    for (; iter != iterEnd; ++iter) {
      const xAOD::RODHeader* header = *iter;
      const int crate = header->crate();
      const int slink = header->sLink();
      const int dataType = header->dataType();
      const int rod = crate + dataType * 6;
      const int pos = rod * 4 + slink;
      if (pos >= 80) continue;
      noFragmentFlags[pos] = 0;
    }
  }

  // Any fragments missing?
  for (int pos = 0; pos < 80; ++pos) {
    if ((pos % 2) && (pos >= 56 || (pos >= 32 && pos < 48))) continue;
    if (noFragmentFlags[pos]) return true;
  }

  return false;
}

// Return true if current event has any ROB or unpacking errors

bool TrigT1CaloMonErrorTool::robOrUnpackingError()
{
  const std::vector<unsigned int>* errVecTES = 0;
  StatusCode sc = retrieve(errVecTES);
  return (sc.isSuccess() && !errVecTES->empty());
}

} // end namespace
