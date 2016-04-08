/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"
#include "DataModel/DataVector.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

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

#include "TrigT1CaloMonErrorToolV1.h"

namespace LVL1 {


// Constructor

TrigT1CaloMonErrorToolV1::TrigT1CaloMonErrorToolV1(
  const std::string& name)
  : AsgTool(name)
{

  declareProperty("TriggerTowerLocation",
                  m_triggerTowerLocation =
                    LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("CPMTowerLocation",
                  m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                  m_cpmTowerLocationOverlap  =
                    LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap");
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
                    LVL1::TrigT1CaloDefs::JetElementLocation + "Overlap");
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

TrigT1CaloMonErrorToolV1::~TrigT1CaloMonErrorToolV1()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode TrigT1CaloMonErrorToolV1::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode TrigT1CaloMonErrorToolV1::finalize()
{
  return StatusCode::SUCCESS;
}

// Retrieve error vector

StatusCode TrigT1CaloMonErrorToolV1::retrieve(const std::vector<unsigned int>*&
    errColl)
{

  // Must ensure bytestream converters have unpacked all our data
  // before retrieving error vector.

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

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0;
  const TriggerTowerCollection* triggerTowerSpareTES = 0;
  const TriggerTowerCollection* triggerTowerMuonTES = 0;
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation);
  if ( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endreq;
  }
  if (evtStore()->contains<TriggerTowerCollection>(m_triggerTowerLocation + "Spare")) {
    sc = evtStore()->retrieve(triggerTowerSpareTES,
                              m_triggerTowerLocation + "Spare");
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !triggerTowerSpareTES ) {
    msg(MSG::DEBUG) << "No Spare Trigger Tower container found" << endreq;
  }
  if (evtStore()->contains<TriggerTowerCollection>(m_triggerTowerLocation + "Muon")) {
    sc = evtStore()->retrieve(triggerTowerMuonTES,
                              m_triggerTowerLocation + "Muon");
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !triggerTowerMuonTES ) {
    msg(MSG::DEBUG) << "No Tile Muon Trigger Tower container found" << endreq;
  }

  //Retrieve Core and Overlap CPM Towers from SG
  const CpmTowerCollection* cpmTowerTES = 0;
  const CpmTowerCollection* cpmTowerOvTES = 0;
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation);
  if ( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found" << endreq;
  }
  if (evtStore()->contains<CpmTowerCollection>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOvTES, m_cpmTowerLocationOverlap);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpmTowerOvTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found" << endreq;
  }

  //Retrieve CPM RoIs from SG
  const CpmRoiCollection* cpmRoiTES = 0;
  sc = evtStore()->retrieve( cpmRoiTES, m_cpmRoiLocation);
  if ( sc.isFailure()  ||  !cpmRoiTES ) {
    msg(MSG::DEBUG) << "No CPM RoIs container found" << endreq;
  }

  //Retrieve CPM Hits from SG
  const CpmHitsCollection* cpmHitsTES = 0;
  sc = evtStore()->retrieve( cpmHitsTES, m_cpmHitsLocation);
  if ( sc.isFailure()  ||  !cpmHitsTES ) {
    msg(MSG::DEBUG) << "No CPM Hits container found" << endreq;
  }

  //Retrieve CMM-CP Hits from SG
  const CmmCpHitsCollection* cmmCpHitsTES = 0;
  sc = evtStore()->retrieve( cmmCpHitsTES, m_cmmCpHitsLocation);
  if ( sc.isFailure()  ||  !cmmCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMM-CP Hits container found" << endreq;
  }

  //Retrieve Core and Overlap Jet Elements from SG
  const JetElementCollection* jetElementTES = 0;
  const JetElementCollection* jetElementOvTES = 0;
  sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation);
  if ( sc.isFailure()  ||  !jetElementTES ) {
    msg(MSG::DEBUG) << "No Core Jet Element container found" << endreq;
  }
  if (evtStore()->contains<JetElementCollection>(m_jetElementLocationOverlap)) {
    sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jetElementOvTES ) {
    msg(MSG::DEBUG) << "No Overlap Jet Element container found" << endreq;
  }

  //Retrieve JEM RoIs from SG
  const JemRoiCollection* jemRoiTES = 0;
  sc = evtStore()->retrieve( jemRoiTES, m_jemRoiLocation);
  if ( sc.isFailure()  ||  !jemRoiTES  ) {
    msg(MSG::DEBUG) << "No DAQ JEM RoIs container found" << endreq;
  }

  //Retrieve JEM Hits from SG
  const JemHitsCollection* jemHitsTES = 0;
  sc = evtStore()->retrieve( jemHitsTES, m_jemHitsLocation);
  if ( sc.isFailure()  ||  !jemHitsTES ) {
    msg(MSG::DEBUG) << "No JEM Hits container found" << endreq;
  }

  //Retrieve CMM-Jet Hits from SG
  const CmmJetHitsCollection* cmmJetHitsTES = 0;
  sc = evtStore()->retrieve( cmmJetHitsTES, m_cmmJetHitsLocation);
  if ( sc.isFailure()  ||  !cmmJetHitsTES ) {
    msg(MSG::DEBUG) << "No CMM-Jet Hits container found" << endreq;
  }

  //Retrieve CMM RoIs from SG
  const LVL1::CMMRoI* cmmRoiTES = 0;
  sc = evtStore()->retrieve( cmmRoiTES, m_cmmRoiLocation);
  if ( sc.isFailure()  ||  !cmmRoiTES ) {
    msg(MSG::DEBUG) << "No CMM RoIs container found" << endreq;
  }

  //Retrieve JEM Et Sums from SG
  const JemEtSumsCollection* jemEtSumsTES = 0;
  sc = evtStore()->retrieve( jemEtSumsTES, m_jemEtSumsLocation);
  if ( sc.isFailure()  ||  !jemEtSumsTES ) {
    msg(MSG::DEBUG) << "No JEM Et Sums container found" << endreq;
  }

  //Retrieve CMM Et Sums from SG
  const CmmEtSumsCollection* cmmEtSumsTES = 0;
  sc = evtStore()->retrieve( cmmEtSumsTES, m_cmmEtSumsLocation);
  if ( sc.isFailure()  ||  !cmmEtSumsTES ) {
    msg(MSG::DEBUG) << "No CMM-Energy Et Sums container found" << endreq;
  }

  //Retrieve ROD Headers from SG
  const RodHeaderCollection* rodTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endreq;
  }

  //Retrieve CP RoIB ROD Headers from SG
  const RodHeaderCollection* cpRoibTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_cpRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpRoibTES ) {
    msg(MSG::DEBUG) << "No CP RoIB ROD Header container found" << endreq;
  }

  //Retrieve JEP RoIB ROD Headers from SG
  const RodHeaderCollection* jepRoibTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_jepRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jepRoibTES ) {
    msg(MSG::DEBUG) << "No JEP RoIB ROD Header container found" << endreq;
  }

  //Retrieve ROB Status and Unpacking Error vector from SG
  errColl = 0;
  if (evtStore()->contains<std::vector<unsigned int> >(m_robErrorVectorLocation)) {
    sc = evtStore()->retrieve(errColl, m_robErrorVectorLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !errColl ) {
    msg(MSG::DEBUG) << "No ROB Status and Unpacking Error vector found"
                    << endreq;
  }

  return sc;
}

// Return true if current event has any corruption errors

bool TrigT1CaloMonErrorToolV1::corrupt()
{
  if (m_flagCorruptEvents == "AnyROBOrUnpackingError") {
    return robOrUnpackingError();
  } else if (m_flagCorruptEvents == "FullEventTimeout") {
    return (fullEventTimeout() && missingFragment() && !robOrUnpackingError());
  }

  return false;
}

// Return true if current event has Full Event status generic timeout bit set

bool TrigT1CaloMonErrorToolV1::fullEventTimeout()
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

bool TrigT1CaloMonErrorToolV1::missingFragment()
{

  typedef DataVector<LVL1::RODHeader> RodHeaderCollection;

  const bool debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  //Retrieve DAQ ROD Headers from SG
  const RodHeaderCollection* rodTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !rodTES ) {
    if (debug) msg(MSG::DEBUG) << "No DAQ ROD Header container found"
                                 << endreq;
  }

  //Retrieve CP RoIB ROD Headers from SG
  const RodHeaderCollection* cpRoibTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_cpRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpRoibTES ) {
    if (debug) msg(MSG::DEBUG) << "No CP RoIB ROD Header container found"
                                 << endreq;
  }

  //Retrieve JEP RoIB ROD Headers from SG
  const RodHeaderCollection* jepRoibTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_jepRoibRodHeaderLocation)) {
    sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !jepRoibTES ) {
    if (debug) msg(MSG::DEBUG) << "No JEP RoIB ROD Header container found"
                                 << endreq;
  }

  // Record fragments present
  std::vector<int> noFragmentFlags(80, 1);
  std::vector<const RodHeaderCollection*> cols;
  if (rodTES)     cols.push_back(rodTES);
  if (cpRoibTES)  cols.push_back(cpRoibTES);
  if (jepRoibTES) cols.push_back(jepRoibTES);
  std::vector<const RodHeaderCollection*>::const_iterator colIter =
    cols.begin();
  std::vector<const RodHeaderCollection*>::const_iterator colIterEnd =
    cols.end();
  for (; colIter != colIterEnd; ++colIter) {
    RodHeaderCollection::const_iterator iter    = (*colIter)->begin();
    RodHeaderCollection::const_iterator iterEnd = (*colIter)->end();
    for (; iter != iterEnd; ++iter) {
      const LVL1::RODHeader* header = *iter;
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

bool TrigT1CaloMonErrorToolV1::robOrUnpackingError()
{
  const std::vector<unsigned int>* errVecTES = 0;
  StatusCode sc = retrieve(errVecTES);
  return (sc.isSuccess() && !errVecTES->empty());
}

} // end namespace
