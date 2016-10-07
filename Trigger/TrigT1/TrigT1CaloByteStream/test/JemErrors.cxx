/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "JemErrors.h"

namespace LVL1BS {

JemErrors::JemErrors(const std::string& name, ISvcLocator* pSvcLocator)
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
    m_doneJeSubStatus(false),
    m_doneJemEtReadoutA(false),
    m_doneJemEtReadoutB(false),
    m_doneJemEtReadoutC(false),
    m_doneJemExReadoutA(false),
    m_doneJemExReadoutB(false),
    m_doneJemExReadoutC(false),
    m_doneJemEyReadoutA(false),
    m_doneJemEyReadoutB(false),
    m_doneJemEyReadoutC(false),
    m_doneCmxTobParity(false),
    m_doneTobLgReadoutA(false),
    m_doneTobLgReadoutB(false),
    m_doneTobLgReadoutC(false),
    m_doneTobSmReadoutA(false),
    m_doneTobSmReadoutB(false),
    m_doneTobSmReadoutC(false),
    m_doneCmxHitsParity0(false),
    m_doneCmxHitsParity1(false),
    m_doneCmxHitsParity2(false),
    m_doneCmxHitsParity3(false)

{
  declareProperty("JetElementLocation",
           m_jetElementLocation = LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JEMEtSumsLocation",
           m_jemEtSumsLocation  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMXJetTobLocation",
           m_cmxJetTobLocation  = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty("CMXJetHitsLocation",
           m_cmxJetHitsLocation = LVL1::TrigT1CaloDefs::CMXJetHitsLocation);
  declareProperty("CMXEtSumsLocation",
           m_cmxEnergyLocation  = LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
  declareProperty("JEMTobRoILocation",
           m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMTobRoILocation);
  declareProperty("CMXRoILocation",
           m_cmxRoiLocation     = LVL1::TrigT1CaloDefs::CMXRoILocation);
  declareProperty("JetElementLocationOut",
           m_jetElementLocationOut = LVL1::TrigT1CaloDefs::JetElementLocation+"Errors");
  declareProperty("JEMEtSumsLocationOut",
           m_jemEtSumsLocationOut  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation+"Errors");
  declareProperty("CMXJetTobLocationOut",
           m_cmxJetTobLocationOut  = LVL1::TrigT1CaloDefs::CMXJetTobLocation+"Errors");
  declareProperty("CMXJetHitsLocationOut",
           m_cmxJetHitsLocationOut = LVL1::TrigT1CaloDefs::CMXJetHitsLocation+"Errors");
  declareProperty("CMXEtSumsLocationOut",
           m_cmxEnergyLocationOut  = LVL1::TrigT1CaloDefs::CMXEtSumsLocation+"Errors");
  declareProperty("JEMTobRoILocationOut",
           m_jemRoiLocationOut     = LVL1::TrigT1CaloDefs::JEMTobRoILocation+"Errors");
  declareProperty("CMXRoILocationOut",
           m_cmxRoiLocationOut     = LVL1::TrigT1CaloDefs::CMXRoILocation+"Errors");

  declareProperty("JetElementErrors", m_jetElementErrors = 0xffff);
  declareProperty("JEMEtSumsErrors",  m_jemEtSumsErrors  = 0xffff);
  declareProperty("CMXJetTobErrors",  m_cmxTobErrors     = 0xffff);
  declareProperty("CMXJetHitsErrors", m_cmxHitsErrors    = 0xffff);
  declareProperty("CMXEtSumsErrors",  m_cmxEtSumsErrors  = 0xffff);
  declareProperty("JEMTobRoIErrors",  m_jemRoiErrors     = 0xffff);
  declareProperty("CMXRoIErrors",     m_cmxRoiErrors     = 0xffff);
}

JemErrors::~JemErrors()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode JemErrors::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  // Initialise individual error flags

  m_doneEmParity    = ((m_jetElementErrors&Error1) == 0);
  m_doneHadParity   = ((m_jetElementErrors&Error2) == 0);
  m_doneEmLinkDown  = ((m_jetElementErrors&Error3) == 0);
  m_doneHadLinkDown = ((m_jetElementErrors&Error4) == 0);
  m_doneEmReadoutA  = ((m_jetElementErrors&Error5) == 0);
  m_doneEmReadoutB  = ((m_jetElementErrors&Error6) == 0);
  m_doneEmReadoutC  = ((m_jetElementErrors&Error7) == 0);
  m_doneHadReadoutA = ((m_jetElementErrors&Error8) == 0);
  m_doneHadReadoutB = ((m_jetElementErrors&Error9) == 0);
  m_doneHadReadoutC = ((m_jetElementErrors&Error10) == 0);
  m_doneJeSubStatus = ((m_jetElementErrors&Error11) == 0);

  m_doneJemEtReadoutA = ((m_jemEtSumsErrors&Error1) == 0);
  m_doneJemEtReadoutB = ((m_jemEtSumsErrors&Error2) == 0);
  m_doneJemEtReadoutC = ((m_jemEtSumsErrors&Error3) == 0);
  m_doneJemExReadoutA = ((m_jemEtSumsErrors&Error4) == 0);
  m_doneJemExReadoutB = ((m_jemEtSumsErrors&Error5) == 0);
  m_doneJemExReadoutC = ((m_jemEtSumsErrors&Error6) == 0);
  m_doneJemEyReadoutA = ((m_jemEtSumsErrors&Error7) == 0);
  m_doneJemEyReadoutB = ((m_jemEtSumsErrors&Error8) == 0);
  m_doneJemEyReadoutC = ((m_jemEtSumsErrors&Error9) == 0);

  m_doneCmxTobParity  = ((m_cmxTobErrors&Error1) == 0);
  m_doneTobLgReadoutA = ((m_cmxTobErrors&Error2) == 0);
  m_doneTobLgReadoutB = ((m_cmxTobErrors&Error3) == 0);
  m_doneTobLgReadoutC = ((m_cmxTobErrors&Error4) == 0);
  m_doneTobSmReadoutA = ((m_cmxTobErrors&Error5) == 0);
  m_doneTobSmReadoutB = ((m_cmxTobErrors&Error6) == 0);
  m_doneTobSmReadoutC = ((m_cmxTobErrors&Error7) == 0);

  m_doneCmxHitsParity0 = ((m_cmxHitsErrors&Error1) == 0);
  m_doneCmxHitsParity1 = ((m_cmxHitsErrors&Error2) == 0);
  m_doneCmxHitsParity2 = ((m_cmxHitsErrors&Error3) == 0);
  m_doneCmxHitsParity3 = ((m_cmxHitsErrors&Error4) == 0);

  return StatusCode::SUCCESS;
}

// Execute

StatusCode JemErrors::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  m_doneThisEvent = false;

  if (m_jetElementErrors) {

    // Find jet elements

    const JetElementCollection* jeCollection = 0;
    StatusCode sc = evtStore()->retrieve(jeCollection, m_jetElementLocation);
    if (sc.isFailure() || !jeCollection || jeCollection->empty()) {
      msg() << "No Jet Elements found" << endmsg;
      jeCollection = 0;
    }

    // Generate jet element errors

    JetElementCollection* errCollection = new JetElementCollection;
    if (jeCollection) {
      jetElementErrors(jeCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_jetElementLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording JetElement container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_jemEtSumsErrors) {

    // Find energy sums

    const EnergySumsCollection* etCollection = 0;
    StatusCode sc = evtStore()->retrieve(etCollection, m_jemEtSumsLocation);
    if (sc.isFailure() || !etCollection || etCollection->empty()) {
      msg() << "No Energy Sums found" << endmsg;
      etCollection = 0;
    }

    // Generate energy sum errors

    EnergySumsCollection* errCollection = new EnergySumsCollection;
    if (etCollection) {
      jemEtSumsErrors(etCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_jemEtSumsLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording JEMEtSums container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_cmxTobErrors) {

    // Find CMX TOBs

    const CmxJetTobCollection* tobCollection = 0;
    StatusCode sc = evtStore()->retrieve(tobCollection, m_cmxJetTobLocation);
    if (sc.isFailure() || !tobCollection || tobCollection->empty()) {
      msg() << "No CMX TOBs found" << endmsg;
      tobCollection = 0;
    }

    // Generate CMX TOB errors

    CmxJetTobCollection* errCollection = new CmxJetTobCollection;
    if (tobCollection) {
      cmxTobErrors(tobCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxJetTobLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXJetTob container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_cmxHitsErrors) {

    // Find CMX hits

    const CmxJetHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmxJetHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMX Hits found" << endmsg;
      hitCollection = 0;
    }

    // Generate CMX Hits errors

    CmxJetHitsCollection* errCollection = new CmxJetHitsCollection;
    if (hitCollection) {
      cmxHitsErrors(hitCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxJetHitsLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXJetHits container in TDS " << endmsg;
      return sc;
    }
  }
/*
  if (m_cmxEtSumsErrors) {

    // Find CMX energy sums

    const CmxEnergyCollection* etCollection = 0;
    StatusCode sc = evtStore()->retrieve(etCollection, m_cmxEnergyLocation);
    if (sc.isFailure() || !etCollection || etCollection->empty()) {
      msg() << "No CMX Energy Sums found" << endmsg;
      etCollection = 0;
    }

    // Generate CMX energy sums errors

    CmxEnergyCollection* errCollection = new CmxEnergyCollection;
    if (etCollection) {
      cmxEtSumsErrors(etCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxEnergyLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXEtSums container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_jemRoiErrors) {

    // Find JEM RoIs

    const JemRoiCollection* jrCollection = 0;
    StatusCode sc = evtStore()->retrieve(jrCollection, m_jemRoiLocation);
    if (sc.isFailure() || !jrCollection || jrCollection->empty()) {
      msg() << "No JEM RoIs found" << endmsg;
      jrCollection = 0;
    }

    // Generate JEM RoI errors

    JemRoiCollection* errCollection = new JemRoiCollection;
    if (jrCollection) {
      jemRoiErrors(jrCollection, errCollection);
    }

    // Save error collection

    sc = evtStore()->record(errCollection, m_jemRoiLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording JEMTobRoI container in TDS " << endmsg;
      return sc;
    }
  }

  if (m_cmxRoiErrors) {

    // Find CMX RoIs

    const LVL1::CMXRoI* crCollection = 0;
    StatusCode sc = evtStore()->retrieve(crCollection, m_cmxRoiLocation);
    if (sc.isFailure() || !crCollection || (!crCollection->roiWord(0) &&
                                            !crCollection->roiWord(1) &&
                                            !crCollection->roiWord(2) &&
                                            !crCollection->roiWord(3) &&
                                            !crCollection->roiWord(4) &&
                                            !crCollection->roiWord(5))) {
      msg() << "No CMX RoIs found" << endmsg;
      crCollection = 0;
    }

    // Generate CMX RoI errors

    CMXRoI* errCollection = 0;
    if (crCollection) {
      cmxRoiErrors(crCollection, errCollection);
    } else errCollection = new CMXRoI();

    // Save error collection

    sc = evtStore()->record(errCollection, m_cmxRoiLocationOut);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Error recording CMXRoI container in TDS " << endmsg;
      return sc;
    }
  }
*/
  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JemErrors::finalize()
{
  return StatusCode::SUCCESS;
}

// Generate JetElement errors

void JemErrors::jetElementErrors(const JetElementCollection* jeCollection,
                                       JetElementCollection* errCollection)
{
  LVL1::CoordToHardware converter;
  JetElementCollection::const_iterator iter    = jeCollection->begin();
  JetElementCollection::const_iterator iterEnd = jeCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::JetElement* const je = *iter;
    const double eta = je->eta();
    const double phi = je->phi();
    const LVL1::Coordinate coord(phi, eta);
    const bool overlap = (converter.jepCrateOverlap(coord) <= 1);
    unsigned int key = je->key();
    const int peak = je->peak();
    std::vector<int> emEnergy(je->emEnergyVec());
    std::vector<int> hadEnergy(je->hadEnergyVec());
    std::vector<int> emError(je->emErrorVec());
    std::vector<int> hadError(je->hadErrorVec());
    std::vector<int> linkError(je->linkErrorVec());
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
	linkError[peak] |= 0x1;
        errorType = "EM Link Down";
        m_doneEmLinkDown = true;
      } else if (!m_doneHadLinkDown) {
        hadError[peak] |= (1 << LVL1::DataError::LinkDown);
	linkError[peak] |= 0x2;
        errorType = "Had Link Down";
        m_doneHadLinkDown = true;
      } else if (!m_doneEmReadoutA && emEnergy[peak] != 0 &&
                 !je->isEmSaturated()) {
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
                 !je->isHadSaturated()) {
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
      } else if (!m_doneJeSubStatus) {
        emError[peak]  |= (1 << LVL1::DataError::BCNMismatch);
        hadError[peak] |= (1 << LVL1::DataError::BCNMismatch);
        errorType = "SubStatus";
        m_doneJeSubStatus = true;
      }
      if (errorType != "") {
        errorMessage("JetElements " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::JetElement* err = new LVL1::JetElement(phi, eta, emEnergy, hadEnergy,
                            key, emError, hadError, linkError, peak);
    errCollection->push_back(err);
  }
}

// Generate JEMEtSums errors

void JemErrors::jemEtSumsErrors(const EnergySumsCollection* etCollection,
                                      EnergySumsCollection* errCollection)
{
  EnergySumsCollection::const_iterator iter    = etCollection->begin();
  EnergySumsCollection::const_iterator iterEnd = etCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::JEMEtSums* const es = *iter;
    const int crate = es->crate();
    const int module = es->module();
    const int peak = es->peak();
    std::vector<unsigned int> et(es->EtVec());
    std::vector<unsigned int> ex(es->ExVec());
    std::vector<unsigned int> ey(es->EyVec());
    if (!m_doneThisEvent) {
      std::string errorType = "";
      if (!m_doneJemEtReadoutA && et[peak] != 0) {
        et[peak] += 1;
        errorType = "Et readout non-zero mismatch";
        m_doneJemEtReadoutA = true;
      } else if (!m_doneJemEtReadoutB && et[peak] == 0) {
        et[peak] = 1;
        errorType = "Et readout sim zero mismatch";
        m_doneJemEtReadoutB = true;
      } else if (!m_doneJemEtReadoutC && et[peak] != 0) {
        et[peak] = 0;
        errorType = "Et readout data zero mismatch";
        m_doneJemEtReadoutC = true;
      } else if (!m_doneJemExReadoutA && ex[peak] != 0) {
        ex[peak] += 1;
        errorType = "Ex readout non-zero mismatch";
        m_doneJemExReadoutA = true;
      } else if (!m_doneJemExReadoutB && ex[peak] == 0) {
        ex[peak] = 1;
        errorType = "Ex readout sim zero mismatch";
        m_doneJemExReadoutB = true;
      } else if (!m_doneJemExReadoutC && ex[peak] != 0) {
        ex[peak] = 0;
        errorType = "Ex readout data zero mismatch";
        m_doneJemExReadoutC = true;
      } else if (!m_doneJemEyReadoutA && ey[peak] != 0) {
        ey[peak] += 1;
        errorType = "Ey readout non-zero mismatch";
        m_doneJemEyReadoutA = true;
      } else if (!m_doneJemEyReadoutB && ey[peak] == 0) {
        ey[peak] = 1;
        errorType = "Ey readout sim zero mismatch";
        m_doneJemEyReadoutB = true;
      } else if (!m_doneJemEyReadoutC && ey[peak] != 0) {
        ey[peak] = 0;
        errorType = "Ey readout data zero mismatch";
        m_doneJemEyReadoutC = true;
      }
      if (errorType != "") {
        errorMessage("JEMEtSums " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::JEMEtSums* err = new LVL1::JEMEtSums(crate, module,
                                               et, ex, ey, peak);
    errCollection->push_back(err);
  }
}

// Generate CMXJetTob errors

void JemErrors::cmxTobErrors(const CmxJetTobCollection* tobCollection,
                                   CmxJetTobCollection* errCollection)
{
  CmxJetTobCollection::const_iterator iter    = tobCollection->begin();
  CmxJetTobCollection::const_iterator iterEnd = tobCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::CMXJetTob* const tob = *iter;
    const int crate = tob->crate();
    const int jem   = tob->jem();
    const int frame = tob->frame();
    const int loc   = tob->location();
    const int peak  = tob->peak();
    std::vector<int> energyLg(tob->energyLgVec());
    std::vector<int> energySm(tob->energySmVec());
    std::vector<int> error(tob->errorVec());
    std::vector<unsigned int> presence(tob->presenceMapVec());
    if (!m_doneThisEvent) {
      std::string errorType = "";
      if (!m_doneCmxTobParity) {
        error[peak] |= (1 << LVL1::DataError::Parity);
        error[peak] |= (1 << LVL1::DataError::ParityPhase1);
        errorType = "CMX-Jet TOB Parity";
        m_doneCmxTobParity = true;
      } else if (!m_doneTobLgReadoutA && energyLg[peak] != 0) {
        energyLg[peak] += 1;
        errorType = "CMX-Jet TOB Energy Large readout non-zero mismatch";
        m_doneTobLgReadoutA = true;
      } else if (!m_doneTobLgReadoutB && energyLg[peak] == 0) { // never happen?
        energyLg[peak] = 1;
        errorType = "CMX-Jet TOB Energy Large readout sim zero mismatch";
        m_doneTobLgReadoutB = true;
      } else if (!m_doneTobLgReadoutC && energyLg[peak] != 0) {
        energyLg[peak] = 0;
        errorType = "CMX-Jet TOB Energy Large readout data zero mismatch";
        m_doneTobLgReadoutC = true;
      } else if (!m_doneTobSmReadoutA && energySm[peak] != 0) {
        energySm[peak] += 1;
        errorType = "CMX-Jet TOB Energy Small readout non-zero mismatch";
        m_doneTobSmReadoutA = true;
      } else if (!m_doneTobSmReadoutB && energySm[peak] == 0) {
        energySm[peak] = 1;
        errorType = "CMX-Jet TOB Energy Small readout sim zero mismatch";
        m_doneTobSmReadoutB = true;
      } else if (!m_doneTobSmReadoutC && energySm[peak] != 0) {
        energySm[peak] = 0;
        errorType = "CMX-Jet TOB Energy Small readout data zero mismatch";
        m_doneTobSmReadoutC = true;
      }
      if (errorType != "") {
        errorMessage("CMXJetTob " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::CMXJetTob* err = new LVL1::CMXJetTob(crate, jem, frame, loc,
                                               energyLg, energySm, error,
					       presence, peak);
    errCollection->push_back(err);
  }
}

// Generate CMXJetHits errors

void JemErrors::cmxHitsErrors(const CmxJetHitsCollection* hitCollection,
                                    CmxJetHitsCollection* errCollection)
{
  CmxJetHitsCollection::const_iterator iter    = hitCollection->begin();
  CmxJetHitsCollection::const_iterator iterEnd = hitCollection->end();
  for (; iter != iterEnd; ++iter) {
    const LVL1::CMXJetHits* const hits = *iter;
    const int crate  = hits->crate();
    const int source = hits->source();
    const int peak   = hits->peak();
    std::vector<unsigned int> hits0(hits->hitsVec0());
    std::vector<unsigned int> hits1(hits->hitsVec1());
    std::vector<int> error0(hits->errorVec0());
    std::vector<int> error1(hits->errorVec1());
    if (!m_doneThisEvent) {
      std::string errorType = "";
      if (!m_doneCmxHitsParity0 && source == LVL1::CMXJetHits::REMOTE_MAIN) {
        error0[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-Jet Hits Parity Cable 0 Phase 0";
        m_doneCmxHitsParity0 = true;
      } else if (!m_doneCmxHitsParity1 && source == LVL1::CMXJetHits::REMOTE_MAIN) {
        error1[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-Jet Hits Parity Cable 0 Phase 1";
        m_doneCmxHitsParity1 = true;
      } else if (!m_doneCmxHitsParity2 && source == LVL1::CMXJetHits::REMOTE_FORWARD) {
        error0[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-Jet Hits Parity Cable 1 Phase 0";
        m_doneCmxHitsParity2 = true;
      } else if (!m_doneCmxHitsParity3 && source == LVL1::CMXJetHits::REMOTE_FORWARD) {
        error1[peak] |= (1 << LVL1::DataError::Parity);
        errorType = "CMX-Jet Hits Parity Cable 1 Phase 1";
        m_doneCmxHitsParity3 = true;
      /*
       * To be continued?
      */
      }
      if (errorType != "") {
        errorMessage("CMXJetHits " + errorType);
        m_doneThisEvent = true;
      }
    }
    LVL1::CMXJetHits* err = new LVL1::CMXJetHits(crate, source, hits0, hits1,
						 error0, error1, peak);
    errCollection->push_back(err);
  }
}

// Print a message when error generated

void JemErrors::errorMessage(const std::string& errmsg)
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
