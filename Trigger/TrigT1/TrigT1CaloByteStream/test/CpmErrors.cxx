/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "CpmErrors.h"

namespace LVL1BS {

CpmErrors::CpmErrors(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_doneThisEvent(false),
    m_doneEmParity(false),
    m_doneHadParity(false),
    m_doneEmLinkDown(false),
    m_doneHadLinkDown(false),
    m_doneEmReadoutA(false),
    m_doneEmReadoutB(false),
    m_doneEmReadoutC(false),
    m_doneHadReadoutA(false),
    m_doneHadReadoutB(false),
    m_doneHadReadoutC(false),
    m_doneCmxTobParityM(false),
    m_doneCmxTobParity0(false),
    m_doneCmxTobParity1(false),
    m_doneCmxTobParity2(false),
    m_doneCmxTobParity3(false),
    m_doneTobEnReadoutA(false),
    m_doneTobEnReadoutB(false),
    m_doneTobEnReadoutC(false),
    m_doneTobIsReadoutA(false),
    m_doneTobIsReadoutB(false),
    m_doneTobIsReadoutC(false),
    m_doneCmxHitsParity0(false),
    m_doneCmxHitsParity1(false)

{
  declareProperty("CPMTowerLocation",
           m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CMXCPTobLocation",
           m_cmxCpTobLocation  = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
  declareProperty("CMXCPHitsLocation",
           m_cmxCpHitsLocation = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);
  declareProperty("CPMTobRoILocation",
           m_cpmTobRoiLocation = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("CPMTowerLocationOut",
           m_cpmTowerLocationOut  = LVL1::TrigT1CaloDefs::CPMTowerLocation+"Errors");
  declareProperty("CMXCPTobLocationOut",
           m_cmxCpTobLocationOut  = LVL1::TrigT1CaloDefs::CMXCPTobLocation+"Errors");
  declareProperty("CMXCPHitsLocationOut",
           m_cmxCpHitsLocationOut = LVL1::TrigT1CaloDefs::CMXCPHitsLocation+"Errors");
  declareProperty("CPMTobRoILocationOut",
           m_cpmTobRoiLocationOut = LVL1::TrigT1CaloDefs::CPMTobRoILocation+"Errors");

  declareProperty("CPMTowerErrors",  m_cpmTowerErrors  = 0xffff);
  declareProperty("CMXCPTobErrors",  m_cmxTobErrors    = 0xffff);
  declareProperty("CMXCPHitsErrors", m_cmxHitsErrors   = 0xffff);
  declareProperty("CPMTobRoIErrors", m_cpmTobRoiErrors = 0xffff);

}

CpmErrors::~CpmErrors()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmErrors::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  // Initialise individual error flags

  m_doneEmParity     = ((m_cpmTowerErrors&Error1) == 0);
  m_doneHadParity    = ((m_cpmTowerErrors&Error2) == 0);
  m_doneEmLinkDown   = ((m_cpmTowerErrors&Error3) == 0);
  m_doneHadLinkDown  = ((m_cpmTowerErrors&Error4) == 0);
  m_doneEmReadoutA   = ((m_cpmTowerErrors&Error5) == 0);
  m_doneEmReadoutB   = ((m_cpmTowerErrors&Error6) == 0);
  m_doneEmReadoutC   = ((m_cpmTowerErrors&Error7) == 0);
  m_doneHadReadoutA  = ((m_cpmTowerErrors&Error8) == 0);
  m_doneHadReadoutB  = ((m_cpmTowerErrors&Error9) == 0);
  m_doneHadReadoutC  = ((m_cpmTowerErrors&Error10) == 0);
  m_doneCpmSubStatus = ((m_cpmTowerErrors&Error11) == 0);

  m_doneCmxTobParityM = ((m_cmxTobErrors&Error1) == 0);
  m_doneCmxTobParity0 = ((m_cmxTobErrors&Error2) == 0);
  m_doneCmxTobParity1 = ((m_cmxTobErrors&Error3) == 0);
  m_doneCmxTobParity2 = ((m_cmxTobErrors&Error4) == 0);
  m_doneCmxTobParity3 = ((m_cmxTobErrors&Error5) == 0);
  m_doneTobEnReadoutA = ((m_cmxTobErrors&Error6) == 0);
  m_doneTobEnReadoutB = ((m_cmxTobErrors&Error7) == 0);
  m_doneTobEnReadoutC = ((m_cmxTobErrors&Error8) == 0);
  m_doneTobIsReadoutA = ((m_cmxTobErrors&Error9) == 0);
  m_doneTobIsReadoutB = ((m_cmxTobErrors&Error10) == 0);
  m_doneTobIsReadoutC = ((m_cmxTobErrors&Error11) == 0);

  m_doneCmxHitsParity0 = ((m_cmxHitsErrors&Error1) == 0);
  m_doneCmxHitsParity1 = ((m_cmxHitsErrors&Error2) == 0);

  return StatusCode::SUCCESS;
}

// Execute

StatusCode CpmErrors::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  m_doneThisEvent = false;

  if (m_cpmTowerErrors) {

    // Find CPM towers

    const CpmTowerCollection* cpCollection = 0;
    StatusCode sc = evtStore()->retrieve(cpCollection, m_cpmTowerLocation);
    if (sc.isFailure() || !cpCollection || cpCollection->empty()) {
      msg() << "No CP Elements found" << endmsg;
      cpCollection = 0;
    }

    // Generate CPM tower errors

    CpmTowerCollection* errCollection = new CpmTowerCollection;
    if (cpCollection) {
      cpmTowerErrors(cpCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cpmTowerLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CPMTower container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_cmxTobErrors) {

    // Find CMX TOBs

    const CmxCpTobCollection* tobCollection = 0;
    StatusCode sc = evtStore()->retrieve(tobCollection, m_cmxCpTobLocation);
    if (sc.isFailure() || !tobCollection || tobCollection->empty()) {
      msg() << "No CMX TOBs found" << endmsg;
      tobCollection = 0;
    }

    // Generate CMX TOB errors

    CmxCpTobCollection* errCollection = new CmxCpTobCollection;
    if (tobCollection) {
      cmxTobErrors(tobCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxCpTobLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXCPTob container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_cmxHitsErrors) {

    // Find CMX hits

    const CmxCpHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmxCpHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMX Hits found" << endmsg;
      hitCollection = 0;
    }

    // Generate CMX Hits errors

    CmxCpHitsCollection* errCollection = new CmxCpHitsCollection;
    if (hitCollection) {
      cmxHitsErrors(hitCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxCpHitsLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXCPHits container in TDS " << endmsg;
      return sc;
    }
  }
/*
  if (m_cpmTobRoiErrors) {

    // Find CPM TOB RoIs

    const CpmTobRoiCollection* crCollection = 0;
    StatusCode sc = evtStore()->retrieve(crCollection, m_cpmTobRoiLocation);
    if (sc.isFailure() || !jcCollection || crCollection->empty()) {
      msg() << "No CPM TOB RoIs found" << endmsg;
      crCollection = 0;
    }

    // Generate CPM RoI errors

    CpmTobRoiCollection* errCollection = new CpmTobRoiCollection;
    if (crCollection) {
      cpmTobRoiErrors(crCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cpmTobRoiLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CPMTobRoI container in TDS " << endmsg;
      return sc;
    }
  }
*/
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpmErrors::finalize()
{
  return StatusCode::SUCCESS;
}

// Generate CPMTower errors

void CpmErrors::cpmTowerErrors(const CpmTowerCollection* cpCollection,
                                     CpmTowerCollection* errCollection)
{
  LVL1::CoordToHardware converter;
  CpmTowerCollection::const_iterator iter    = cpCollection->begin();
  CpmTowerCollection::const_iterator iterEnd = cpCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::CPMTower* const cp = *iter;
    const double eta = cp->eta();
    const double phi = cp->phi();
    const LVL1::Coordinate coord(phi, eta);
    const bool overlap = (converter.cpCrateOverlap(coord) <= 3);
    const int peak = cp->peak();
    std::vector<int> emEnergy(cp->emEnergyVec());
    std::vector<int> hadEnergy(cp->hadEnergyVec());
    std::vector<int> emError(cp->emErrorVec());
    std::vector<int> hadError(cp->hadErrorVec());
    if (!m_doneThisEvent && !overlap) {
      std::string errorType = "";
      if (!m_doneEmParity) {
        emError[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "EM Parity";
        m_doneEmParity = true;
      } else if (!m_doneHadParity) {
        hadError[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "Had Parity";
        m_doneHadParity = true;
      } else if (!m_doneEmLinkDown) {
        emError[peak] |= (1 << LVL1::DataError::LinkDown);
        errorType = "EM Link Down";
        m_doneEmLinkDown = true;
      } else if (!m_doneHadLinkDown) {
        hadError[peak] |= (1 << LVL1::DataError::LinkDown);
        errorType = "Had Link Down";
        m_doneHadLinkDown = true;
      } else if (!m_doneEmReadoutA && emEnergy[peak] != 0 &&
                 emEnergy[peak] < (s_saturation-1)) {
        emEnergy[peak] += 1;
        errorType = "EM readout non-zero mismatch";
        m_doneEmReadoutA = true;
      } else if (!m_doneEmReadoutB && emEnergy[peak] == 0) {
        emEnergy[peak] = 1;
        errorType = "EM readout sim zero mismatch";
        m_doneEmReadoutB = true;
      } else if (!m_doneEmReadoutC && emEnergy[peak] != 0 &&
                                      hadEnergy[peak] != 0) {
        emEnergy[peak] = 0;
        errorType = "EM readout data zero mismatch";
        m_doneEmReadoutC = true;
      } else if (!m_doneHadReadoutA && hadEnergy[peak] != 0 &&
                 hadEnergy[peak] < (s_saturation-1)) {
        hadEnergy[peak] += 1;
        errorType = "Had readout non-zero mismatch";
        m_doneHadReadoutA = true;
      } else if (!m_doneHadReadoutB && hadEnergy[peak] == 0) {
        hadEnergy[peak] = 1;
        errorType = "Had readout sim zero mismatch";
        m_doneHadReadoutB = true;
      } else if (!m_doneHadReadoutC && hadEnergy[peak] != 0 &&
                                       emEnergy[peak] != 0) {
        hadEnergy[peak] = 0;
        errorType = "Had readout data zero mismatch";
        m_doneHadReadoutC = true;
      } else if (!m_doneCpmSubStatus) {
        emError[peak]  |= (1 << LVL1::DataError::BCNMismatch);
        hadError[peak] |= (1 << LVL1::DataError::BCNMismatch);
        errorType = "SubStatus";
        m_doneCpmSubStatus = true;
      }
      if (errorType != "") {
        errorMessage("CPMTowers " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::CPMTower* err = new LVL1::CPMTower(phi, eta, emEnergy, emError,
                                             hadEnergy, hadError, peak);
    errCollection->push_back(err);
  }
}

// Generate CMXCPTob errors

void CpmErrors::cmxTobErrors(const CmxCpTobCollection* tobCollection,
                                   CmxCpTobCollection* errCollection)
{
  CmxCpTobCollection::const_iterator iter    = tobCollection->begin();
  CmxCpTobCollection::const_iterator iterEnd = tobCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::CMXCPTob* const tob = *iter;
    const int crate = tob->crate();
    const int cmx   = tob->cmx();
    const int cpm   = tob->cpm();
          int chip  = tob->chip();
          int loc   = tob->location();
    const int peak  = tob->peak();
    std::vector<int> energy(tob->energyVec());
    std::vector<int> isolation(tob->isolationVec());
    std::vector<int> error(tob->errorVec());
    std::vector<unsigned int> presence(tob->presenceMapVec());
    if (!m_doneThisEvent) {
      std::string errorType = "";
      if (!m_doneCmxTobParityM && chip != 0) {
	// NB. Changes real data path
        error[peak] |= (1 << LVL1::DataError::ParityMerge);
	chip = 0;
	loc  = 0;
	energy[peak] = 0;
	isolation[peak] = 0;
        errorType = "CMX-CP TOB Parity Merge";
	m_doneCmxTobParityM = true;
      } else if (!m_doneCmxTobParity0) {
        error[peak] |= (1 << LVL1::DataError::ParityPhase0);
        errorType = "CMX-CP TOB Parity Phase 0";
        m_doneCmxTobParity0 = true;
      } else if (!m_doneCmxTobParity1) {
        error[peak] |= (1 << LVL1::DataError::ParityPhase1);
        errorType = "CMX-CP TOB Parity Phase 1";
        m_doneCmxTobParity1 = true;
      } else if (!m_doneCmxTobParity2) {
        error[peak] |= (1 << LVL1::DataError::ParityPhase2);
        errorType = "CMX-CP TOB Parity Phase 2";
        m_doneCmxTobParity2 = true;
      } else if (!m_doneCmxTobParity3) {
        error[peak] |= (1 << LVL1::DataError::ParityPhase3);
        errorType = "CMX-CP TOB Parity Phase 3";
        m_doneCmxTobParity3 = true;
      } else if (!m_doneTobEnReadoutA && energy[peak] != 0) {
        energy[peak] += 1;
        errorType = "CMX-CP TOB Energy readout non-zero mismatch";
        m_doneTobEnReadoutA = true;
      } else if (!m_doneTobEnReadoutB && energy[peak] == 0) { // never happen?
        energy[peak] = 1;
        errorType = "CMX-CP TOB Energy readout sim zero mismatch";
        m_doneTobEnReadoutB = true;
      } else if (!m_doneTobEnReadoutC && energy[peak] != 0) {
        energy[peak] = 0;
        errorType = "CMX-CP TOB Energy readout data zero mismatch";
        m_doneTobEnReadoutC = true;
      } else if (!m_doneTobIsReadoutA && isolation[peak] != 0) {
        isolation[peak] += 1;
        errorType = "CMX-CP TOB Isolation readout non-zero mismatch";
        m_doneTobIsReadoutA = true;
      } else if (!m_doneTobIsReadoutB && isolation[peak] == 0) {
        isolation[peak] = 1;
        errorType = "CMX-CP TOB Isolation readout sim zero mismatch";
        m_doneTobIsReadoutB = true;
      } else if (!m_doneTobIsReadoutC && isolation[peak] != 0) {
        isolation[peak] = 0;
        errorType = "CMX-CP TOB Isolation readout data zero mismatch";
        m_doneTobIsReadoutC = true;
      }
      if (errorType != "") {
        errorMessage("CMXCPTob " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::CMXCPTob* err = new LVL1::CMXCPTob(crate, cmx, cpm, chip, loc,
                                             energy, isolation, error,
					     presence, peak);
    errCollection->push_back(err);
  }
}

// Generate CMXCPHits errors

void CpmErrors::cmxHitsErrors(const CmxCpHitsCollection* hitCollection,
                                    CmxCpHitsCollection* errCollection)
{
  CmxCpHitsCollection::const_iterator iter    = hitCollection->begin();
  CmxCpHitsCollection::const_iterator iterEnd = hitCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::CMXCPHits* const hits = *iter;
    const int crate  = hits->crate();
    const int cmx    = hits->cmx();
    const int source = hits->source();
    const int peak   = hits->peak();
    std::vector<unsigned int> hits0(hits->hitsVec0());
    std::vector<unsigned int> hits1(hits->hitsVec1());
    std::vector<int> error0(hits->errorVec0());
    std::vector<int> error1(hits->errorVec1());
    if (!m_doneThisEvent) {
      std::string errorType = "";
      if (!m_doneCmxHitsParity0 && source == LVL1::CMXCPHits::REMOTE_0) {
        error0[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-CP Hits Parity Phase 0";
        m_doneCmxHitsParity0 = true;
      } else if (!m_doneCmxHitsParity1 && source == LVL1::CMXCPHits::REMOTE_2) {
        error1[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-CP Hits Parity Phase 1";
        m_doneCmxHitsParity1 = true;
      }
      /*
       * Add hit readout errors here
      */
      if (errorType != "") {
        errorMessage("CMXCPHits " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::CMXCPHits* err = new LVL1::CMXCPHits(crate, cmx, source,
                                               hits0, hits1, error0, error1,
					       peak);
    errCollection->push_back(err);
  }
}

// Generate CPM TOB RoI errors

void CpmErrors::cpmTobRoiErrors(const CpmTobRoiCollection* /*crCollection*/,
                                      CpmTobRoiCollection* /*errCollection*/)
{
}

// Print a message when error generated

void CpmErrors::errorMessage(const std::string& errmsg)
{
  int eventNumber = 0;
  const EventInfo* evInfo = 0;
  StatusCode sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "No EventInfo found" << endmsg;
  } else {
    const EventID* evID = evInfo->event_ID();
    if (evID) eventNumber = evID->event_number();
  }
  msg(MSG::INFO) << "Event " << eventNumber
                 << " has error " << errmsg << endmsg;
}

} // end namespace
