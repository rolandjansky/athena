/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CMXEtSums.h"
#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "JemTesterV2.h"
#include "../src/ModifySlices.h"

namespace LVL1BS {

JemTesterV2::JemTesterV2(const std::string& name, ISvcLocator* pSvcLocator)
                       : AthAlgorithm(name, pSvcLocator),
		         m_elementKey(0)
{
  declareProperty("JetElementLocation",
           m_jetElementLocation = LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JEMEtSumsLocation",
           m_jemEtSumsLocation  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMXJetTobLocation",
           m_cmxTobLocation     = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty("CMXJetHitsLocation",
           m_cmxJetLocation     = LVL1::TrigT1CaloDefs::CMXJetHitsLocation);
  declareProperty("CMXEtSumsLocation",
           m_cmxEnergyLocation  = LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
  declareProperty("JEMTobRoILocation",
           m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMTobRoILocation);
  declareProperty("CMXRoILocation",
           m_cmxRoiLocation     = LVL1::TrigT1CaloDefs::CMXRoILocation);
  declareProperty("JEMTobRoILocationRoIB",
           m_jemRoiLocationRoib =
	                 LVL1::TrigT1CaloDefs::JEMTobRoILocation + "RoIB");
  declareProperty("CMXRoILocationRoIB",
           m_cmxRoiLocationRoib =
	                 LVL1::TrigT1CaloDefs::CMXRoILocation + "RoIB");
  declareProperty("JetElementLocationOverlap",
           m_jetElementLocationOverlap =
	                 LVL1::TrigT1CaloDefs::JetElementLocation + "Overlap");

  declareProperty("ForceSlicesJEM",  m_forceSlicesJem  = 0);
  declareProperty("ForceSlicesCMX",  m_forceSlicesCmx  = 0);

  // By default print everything except RoIB RoIs and jet element overlap
  declareProperty("JetElementPrint", m_jetElementPrint = 1);
  declareProperty("JEMEtSumsPrint",  m_jemEtSumsPrint  = 1);
  declareProperty("CMXJetTobPrint",  m_cmxTobPrint     = 1,
                  "Set to 2 to print presence maps (Neutral format only)");
  declareProperty("CMXJetHitsPrint", m_cmxHitsPrint    = 1);
  declareProperty("CMXEtSumsPrint",  m_cmxEtSumsPrint  = 1);
  declareProperty("JEMRoIPrint",     m_jemRoiPrint     = 1);
  declareProperty("CMXRoIPrint",     m_cmxRoiPrint     = 1);
  declareProperty("JEMRoIPrintRoIB", m_jemRoiPrintRoib = 0);
  declareProperty("CMXRoIPrintRoIB", m_cmxRoiPrintRoib = 0);
  declareProperty("JetElementPrintOverlap", m_jetElementPrintOverlap = 0);
}

JemTesterV2::~JemTesterV2()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode JemTesterV2::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  m_elementKey = new LVL1::JetElementKey();

  return StatusCode::SUCCESS;
}

// Execute

StatusCode JemTesterV2::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  if (m_jetElementPrint) {

    // Find core jet elements

    const JetElementCollection* jeCollection = 0;
    StatusCode sc = evtStore()->retrieve(jeCollection, m_jetElementLocation);
    if (sc.isFailure() || !jeCollection || jeCollection->empty()) {
      msg() << "No core Jet Elements found" << endmsg;
    } else {

      // Order by eta, phi

      setupJeMap(jeCollection);

      // Print the jet elements

      printJetElements("core");
    }
  }

  if (m_jetElementPrintOverlap) {

    // Find overlap jet elements

    const JetElementCollection* jeCollection = 0;
    StatusCode sc = evtStore()->retrieve(jeCollection,
                                      m_jetElementLocationOverlap);
    if (sc.isFailure() || !jeCollection || jeCollection->empty()) {
      msg() << "No overlap Jet Elements found" << endmsg;
    } else {

      // Order by eta, phi

      setupJeMap(jeCollection);

      // Print the jet elements

      printJetElements("overlap");
    }
  }

  if (m_jemEtSumsPrint) {

    // Find energy sums

    const EnergySumsCollection* etCollection = 0;
    StatusCode sc = evtStore()->retrieve(etCollection, m_jemEtSumsLocation);
    if (sc.isFailure() || !etCollection || etCollection->empty()) {
      msg() << "No Energy Sums found" << endmsg;
    } else {

      // Order by crate, module

      setupEtMap(etCollection);

      // Print the energy sums

      printEnergySums();
    }
  }

  if (m_cmxTobPrint) {

    // Find CMX TOBs

    const CmxTobCollection* tobCollection = 0;
    StatusCode sc = evtStore()->retrieve(tobCollection, m_cmxTobLocation);
    if (sc.isFailure() || !tobCollection || tobCollection->empty()) {
      msg() << "No CMX TOBs found" << endmsg;
    } else {

      // Order by crate, jem, frame, loc

      setupCmxTobMap(tobCollection);

      // Print the CMX TOBs

      printCmxTobs();
    }
  }

  if (m_cmxHitsPrint) {

    // Find CMX hits

    const CmxJetCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmxJetLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMX Hits found" << endmsg;
    } else {

      // Order by crate, source

      setupCmxHitsMap(hitCollection);

      // Print the CMX hits

      printCmxHits();
    }
  }

  if (m_cmxEtSumsPrint) {

    // Find CMX energy sums

    const CmxEnergyCollection* etCollection = 0;
    StatusCode sc = evtStore()->retrieve(etCollection, m_cmxEnergyLocation);
    if (sc.isFailure() || !etCollection || etCollection->empty()) {
      msg() << "No CMX Energy Sums found" << endmsg;
    } else {

      // Order by crate, source

      setupCmxEtMap(etCollection);

      // Print the CMX energy sums

      printCmxSums();
    }
  }

  if (m_jemRoiPrint) {

    // Find JEM RoIs

    const JemRoiCollection* jrCollection = 0;
    StatusCode sc = evtStore()->retrieve(jrCollection, m_jemRoiLocation);
    if (sc.isFailure() || !jrCollection || jrCollection->empty()) {
      msg() << "No JEM RoIs found" << endmsg;
    } else {

      // Order by RoI word

      setupJemRoiMap(jrCollection);

      // Print the JEM RoIs

      printJemRois("DAQ");
    }
  }

  if (m_jemRoiPrintRoib) {

    // Find JEM RoIs from RoIB

    const JemRoiCollection* jrCollection = 0;
    StatusCode sc = evtStore()->retrieve(jrCollection, m_jemRoiLocationRoib);
    if (sc.isFailure() || !jrCollection || jrCollection->empty()) {
      msg() << "No JEM RoIs from RoIB found" << endmsg;
    } else {

      // Order by RoI word

      setupJemRoiMap(jrCollection);

      // Print the JEM RoIs from RoIB

      printJemRois("RoIB");
    }
  }

  if (m_cmxRoiPrint) {

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
    } else {

      // Print the CMX RoIs

      printCmxRois("DAQ", crCollection);
    }
  }

  if (m_cmxRoiPrintRoib) {

    // Find CMX RoIs from RoIB

    const LVL1::CMXRoI* crCollection = 0;
    StatusCode sc = evtStore()->retrieve(crCollection, m_cmxRoiLocationRoib);
    if (sc.isFailure() || !crCollection || (!crCollection->roiWord(0) &&
                                            !crCollection->roiWord(1) &&
                                            !crCollection->roiWord(2) &&
                                            !crCollection->roiWord(3) &&
                                            !crCollection->roiWord(4) &&
                                            !crCollection->roiWord(5))) {
      msg() << "No CMX RoIs from RoIB found" << endmsg;
    } else {

      // Print the CMX RoIs from RoIB

      printCmxRois("RoIB", crCollection);
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JemTesterV2::finalize()
{
  delete m_elementKey;

  return StatusCode::SUCCESS;
}

// Print the jet elements

void JemTesterV2::printJetElements(const std::string& source) const
{
  msg() << "Number of " << source << " Jet Elements = "
        << m_jeMap.size() << endmsg;
  JetElementMap::const_iterator mapIter = m_jeMap.begin();
  JetElementMap::const_iterator mapEnd  = m_jeMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::JetElement* const je = mapIter->second;
    int peak   = je->peak();
    int slices = (je->emEnergyVec()).size();
    if (m_forceSlicesJem) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesJem);
      slices = m_forceSlicesJem;
    }
    std::vector<int> emEnergy;
    std::vector<int> hadEnergy;
    std::vector<int> emError;
    std::vector<int> hadError;
    std::vector<int> linkError;
    ModifySlices::data(je->emEnergyVec(),  emEnergy,  slices);
    ModifySlices::data(je->hadEnergyVec(), hadEnergy, slices);
    ModifySlices::data(je->emErrorVec(),   emError,   slices);
    ModifySlices::data(je->hadErrorVec(),  hadError,  slices);
    ModifySlices::data(je->linkErrorVec(), linkError, slices);
    msg() << "key/eta/phi/peak/em/had/emErr/hadErr/linkErr: "
          << mapIter->first << "/" << je->eta() << "/" << je->phi() << "/"
	  << peak << "/";
    printVec(emEnergy);
    printVec(hadEnergy);
    msg() << MSG::hex;
    printVec(emError);
    printVec(hadError);
    printVec(linkError);
    msg() << MSG::dec << endmsg;
  }
}

// Print energy sums

void JemTesterV2::printEnergySums() const
{
  msg() << "Number of Energy Sums = " << m_etMap.size() << endmsg;
  EnergySumsMap::const_iterator mapIter = m_etMap.begin();
  EnergySumsMap::const_iterator mapEnd  = m_etMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::JEMEtSums* const et = mapIter->second;
    int peak   = et->peak();
    int slices = (et->ExVec()).size();
    if (m_forceSlicesJem) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesJem);
      slices = m_forceSlicesJem;
    }
    msg() << "crate/module/peak/Ex/Ey/Et: "
	  << et->crate() << "/" << et->module() << "/" << peak << "/";
    std::vector<unsigned int> exVec;
    std::vector<unsigned int> eyVec;
    std::vector<unsigned int> etVec;
    ModifySlices::data(et->ExVec(), exVec, slices);
    ModifySlices::data(et->EyVec(), eyVec, slices);
    ModifySlices::data(et->EtVec(), etVec, slices);
    printVecU(exVec);
    printVecU(eyVec);
    printVecU(etVec);
    msg() << endmsg;
  }
}

// Print the CMX-Jet TOBs

void JemTesterV2::printCmxTobs() const
{
  msg() << "Number of CMX-Jet TOBs = " << m_cmxTobMap.size() << endmsg;
  CmxTobMap::const_iterator mapIter = m_cmxTobMap.begin();
  CmxTobMap::const_iterator mapEnd  = m_cmxTobMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMXJetTob* const tb = mapIter->second;
    int peak   = tb->peak();
    int slices = (tb->energyLgVec()).size();
    if (m_forceSlicesCmx) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmx);
      slices = m_forceSlicesCmx;
    }
    if (m_cmxTobPrint == 2) {
      msg() << "crate/jem/frame/loc/peak/energyLg/energySm/error/presenceMap: ";
    } else {
      msg() << "crate/jem/frame/loc/peak/energyLg/energySm/error: ";
    }
    msg() << tb->crate() << "/" << tb->jem() << "/" << tb->frame() << "/"
          << tb->location() << "/" << peak << "/";

    std::vector<int> energyLg;
    std::vector<int> energySm;
    std::vector<int> error;
    ModifySlices::data(tb->energyLgVec(), energyLg, slices);
    ModifySlices::data(tb->energySmVec(), energySm, slices);
    ModifySlices::data(tb->errorVec(), error, slices);
    printVec(energyLg);
    printVec(energySm);
    msg() << MSG::hex;
    printVec(error);
    if (m_cmxTobPrint == 2) {
      std::vector<unsigned int> presence;
      ModifySlices::data(tb->presenceMapVec(), presence, slices);
      printVecH(presence, 1, 16);
    }
    msg() << MSG::dec << endmsg;
  }
}

// Print the CMX hits

void JemTesterV2::printCmxHits() const
{
  msg() << "Number of CMX Hits = " << m_cmxHitsMap.size() << endmsg;
  CmxHitsMap::const_iterator mapIter = m_cmxHitsMap.begin();
  CmxHitsMap::const_iterator mapEnd  = m_cmxHitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMXJetHits* const jh = mapIter->second;
    const int source = jh->source();
    int peak   = jh->peak();
    int slices = (jh->hitsVec0()).size();
    if (m_forceSlicesCmx) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmx);
      slices = m_forceSlicesCmx;
    }
    msg() << "crate/source/peak/hits0/hits1/error0/error1: "
	  << jh->crate() << "/" << source << "/" << peak << "/";
    int words1 = 5;
    int words2 = 5;
    int bits   = 3;
    unsigned int mask = 0x7;
    if (source == LVL1::CMXJetHits::REMOTE_FORWARD ||
        source == LVL1::CMXJetHits::LOCAL_FORWARD  ||
        source == LVL1::CMXJetHits::TOTAL_FORWARD) {
      words1 = 8;
      words2 = 7;
      bits   = 2;
      mask   = 0x3;
    } else if (source == LVL1::CMXJetHits::TOPO_CHECKSUM) {
      words1 = 1;
      words2 = 0;
      bits   = 16;
      mask   = 0xffff;
    } else if (source == LVL1::CMXJetHits::TOPO_OCCUPANCY_MAP) {
      words1 = 16;
      words2 = 0;
      bits   = 1;
      mask   = 0x1;
    } else if (source == LVL1::CMXJetHits::TOPO_OCCUPANCY_COUNTS) {
      words1 = 8;
      words2 = 8;
    }
    std::vector<unsigned int> hitsVec0;
    std::vector<unsigned int> hitsVec1;
    ModifySlices::data(jh->hitsVec0(), hitsVec0, slices);
    ModifySlices::data(jh->hitsVec1(), hitsVec1, slices);
    std::vector<unsigned int>::const_iterator pos;
    std::vector<unsigned int>::const_iterator posb = hitsVec0.begin();
    std::vector<unsigned int>::const_iterator pose = hitsVec0.end();
    for (pos = posb; pos != pose; ++pos) {
      if (pos != posb) msg() << ",";
      const unsigned int hits = *pos;
      for (int i = 0; i < words1; ++i) {
        if (i != 0) msg() << ":";
	const unsigned int thr = (hits >> bits*i) & mask;
        msg() << thr;
      }
    }
    msg() << "/";
    posb = hitsVec1.begin();
    pose = hitsVec1.end();
    for (pos = posb; pos != pose; ++pos) {
      if (pos != posb) msg() << ",";
      const unsigned int hits = *pos;
      for (int i = 0; i < words2; ++i) {
        if (i != 0) msg() << ":";
	const unsigned int thr = (hits >> bits*i) & mask;
        msg() << thr;
      }
    }
    msg() << "/" << MSG::hex;
    std::vector<int> errorVec0;
    std::vector<int> errorVec1;
    ModifySlices::data(jh->errorVec0(), errorVec0, slices);
    ModifySlices::data(jh->errorVec1(), errorVec1, slices);
    printVec(errorVec0);
    printVec(errorVec1);
    msg() << MSG::dec << endmsg;
  }
}

// Print CMX energy sums

void JemTesterV2::printCmxSums() const
{
  msg() << "Number of CMX Energy Sums = " << m_cmxEtMap.size() << endmsg;
  CmxSumsMap::const_iterator mapIter = m_cmxEtMap.begin();
  CmxSumsMap::const_iterator mapEnd  = m_cmxEtMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMXEtSums* const et = mapIter->second;
    int peak   = et->peak();
    int slices = (et->ExVec()).size();
    if (m_forceSlicesCmx) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmx);
      slices = m_forceSlicesCmx;
    }
    msg() << "crate/source/peak/Ex/Ey/Et/ExErr/EyErr/EtErr: "
	  << et->crate() << "/" << et->source() << "/" << peak << "/";
    std::vector<unsigned int> exVec;
    std::vector<unsigned int> eyVec;
    std::vector<unsigned int> etVec;
    std::vector<int> exError;
    std::vector<int> eyError;
    std::vector<int> etError;
    ModifySlices::data(et->ExVec(), exVec, slices);
    ModifySlices::data(et->EyVec(), eyVec, slices);
    ModifySlices::data(et->EtVec(), etVec, slices);
    ModifySlices::data(et->ExErrorVec(), exError, slices);
    ModifySlices::data(et->EyErrorVec(), eyError, slices);
    ModifySlices::data(et->EtErrorVec(), etError, slices);
    printVecU(exVec);
    printVecU(eyVec);
    printVecU(etVec);
    msg() << MSG::hex;
    printVec(exError);
    printVec(eyError);
    printVec(etError);
    msg() << MSG::dec << endmsg;
  }
}

// Print the JEM RoIs

void JemTesterV2::printJemRois(const std::string& source) const
{
  msg() << "Number of JEM RoIs (" << source << ") = " << m_roiMap.size()
        << endmsg;
  JemRoiMap::const_iterator mapIter = m_roiMap.begin();
  JemRoiMap::const_iterator mapEnd  = m_roiMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::JEMTobRoI* const roi = mapIter->second;
    msg() << "crate/jem/frame/loc/energyLg/energySm: "
          << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	  << roi->location() << "/" << roi->energyLarge() << "/"
	  << roi->energySmall() << "/" << endmsg;
  }
}

// Print the CMX RoIs

void JemTesterV2::printCmxRois(const std::string& source,
                               const LVL1::CMXRoI* const roi) const
{
  msg() << "CMXRoI(" << source
        << "):Normal:sumEtHits/missingEtHits/missingEtSigHits/Ex/Ey/Et;error: "
        << MSG::hex
	<< roi->sumEtHits()        << "/"
        << roi->missingEtHits()    << "/"
        << roi->missingEtSigHits() << "/"
        << MSG::dec
	<< roi->ex() << ";" << roi->exError() << "/"
        << roi->ey() << ";" << roi->eyError() << "/"
        << roi->et() << ";" << roi->etError() << "/" << endmsg;
  msg() << "CMXRoI(" << source
        << "):Masked:sumEtHits/missingEtHits/Ex/Ey/Et;error: "
        << MSG::hex
	<< roi->sumEtHits(LVL1::CMXRoI::MASKED)     << "/"
        << roi->missingEtHits(LVL1::CMXRoI::MASKED) << "/"
        << MSG::dec
	<< roi->ex(LVL1::CMXRoI::MASKED)            << ";"
	<< roi->exError(LVL1::CMXRoI::MASKED)       << "/"
        << roi->ey(LVL1::CMXRoI::MASKED)            << ";"
	<< roi->eyError(LVL1::CMXRoI::MASKED)       << "/"
        << roi->et(LVL1::CMXRoI::MASKED)            << ";"
	<< roi->etError(LVL1::CMXRoI::MASKED)       << "/" << endmsg;
}

// Print a vector

void JemTesterV2::printVec(const std::vector<int>& vec) const
{
  std::vector<int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Print a vector (unsigned)

void JemTesterV2::printVecU(const std::vector<unsigned int>& vec) const
{
  std::vector<unsigned int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Print a vector of hits

void JemTesterV2::printVecH(const std::vector<unsigned int>& vec,
                            const int bits, const int words) const
{
  const unsigned int mask = (1<<bits)-1;
  std::vector<unsigned int>::const_iterator pos;
  std::vector<unsigned int>::const_iterator posb = vec.begin();
  std::vector<unsigned int>::const_iterator pose = vec.end();
  for (pos = posb; pos != pose; ++pos) {
    if (pos != posb) msg() << ",";
    const unsigned int hits = *pos;
    for (int i = 0; i < words; ++i) {
      if (i != 0) msg() << ":";
      const unsigned int thr = (hits >> (bits*i)) & mask;
      msg() << thr;
    }
  }
  msg() << "/";
}

// Set up jet element map

void JemTesterV2::setupJeMap(const JetElementCollection* const jeCollection)
{
  m_jeMap.clear();
  if (jeCollection) {
    JetElementCollection::const_iterator pos = jeCollection->begin();
    JetElementCollection::const_iterator pose = jeCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JetElement* const je = *pos;
      const unsigned int key = m_elementKey->jeKey(je->phi(), je->eta());
      m_jeMap.insert(std::make_pair(key, je));
    }
  }
}

// Set up energy sums map

void JemTesterV2::setupEtMap(const EnergySumsCollection* const etCollection)
{
  m_etMap.clear();
  if (etCollection) {
    EnergySumsCollection::const_iterator pos  = etCollection->begin();
    EnergySumsCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMEtSums* const sums = *pos;
      // Sim doesn't zero suppress properly, do it here
      if (std::accumulate((sums->ExVec()).begin(), (sums->ExVec()).end(), 0) ||
          std::accumulate((sums->EyVec()).begin(), (sums->EyVec()).end(), 0) ||
          std::accumulate((sums->EtVec()).begin(), (sums->EtVec()).end(), 0)) {
        const int key = sums->crate()*100 + sums->module();
        m_etMap.insert(std::make_pair(key, sums));
      }
    }
  }
}

// Set up CMX TOBs map

void JemTesterV2::setupCmxTobMap(const CmxTobCollection* const tobCollection)
{
  m_cmxTobMap.clear();
  if (tobCollection) {
    CmxTobCollection::const_iterator pos  = tobCollection->begin();
    CmxTobCollection::const_iterator pose = tobCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXJetTob* const tob = *pos;
      const int key = ((((((tob->crate()<<4)+tob->jem())<<3)+tob->frame())<<2)
                                                            +tob->location());
      m_cmxTobMap.insert(std::make_pair(key, tob));
    }
  }
}

// Set up CMX hits map

void JemTesterV2::setupCmxHitsMap(const CmxJetCollection* const hitCollection)
{
  m_cmxHitsMap.clear();
  if (hitCollection) {
    CmxJetCollection::const_iterator pos  = hitCollection->begin();
    CmxJetCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXJetHits* const hits = *pos;
      const int key = hits->crate()*100 + hits->source();
      m_cmxHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMX energy sums map

void JemTesterV2::setupCmxEtMap(const CmxEnergyCollection* const etCollection)
{
  m_cmxEtMap.clear();
  if (etCollection) {
    CmxEnergyCollection::const_iterator pos  = etCollection->begin();
    CmxEnergyCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXEtSums* const sums = *pos;
      if (std::accumulate((sums->ExVec()).begin(), (sums->ExVec()).end(), 0) ||
          std::accumulate((sums->EyVec()).begin(), (sums->EyVec()).end(), 0) ||
          std::accumulate((sums->EtVec()).begin(), (sums->EtVec()).end(), 0) ||
          std::accumulate((sums->ExErrorVec()).begin(),
	                                      (sums->ExErrorVec()).end(), 0) ||
          std::accumulate((sums->EyErrorVec()).begin(),
	                                      (sums->EyErrorVec()).end(), 0) ||
          std::accumulate((sums->EtErrorVec()).begin(),
	                                      (sums->EtErrorVec()).end(), 0)) {
        const int key = sums->crate()*100 + sums->source();
        m_cmxEtMap.insert(std::make_pair(key, sums));
      }
    }
  }
}

// Set up JEM RoI map

void JemTesterV2::setupJemRoiMap(const JemRoiCollection* const jrCollection)
{
  m_roiMap.clear();
  if (jrCollection) {
    JemRoiCollection::const_iterator pos  = jrCollection->begin();
    JemRoiCollection::const_iterator pose = jrCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMTobRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

} // end namespace
