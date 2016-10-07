/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <utility>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMTobRoI.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "CpmTesterV2.h"
#include "../src/ModifySlices.h"

namespace LVL1BS {

CpmTesterV2::CpmTesterV2(const std::string& name, ISvcLocator* pSvcLocator)
                       : AthAlgorithm(name, pSvcLocator),
		         m_towerKey(0)
{
  declareProperty("CPMTowerLocation",
           m_cpmTowerLocation   = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CMXCPTobLocation",
           m_cmxCpTobLocation   = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
  declareProperty("CMXCPHitsLocation",
           m_cmxCpHitsLocation  = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);
  declareProperty("CPMTobRoILocation",
           m_cpmRoiLocation     = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("CPMTobRoILocationRoIB",
           m_cpmRoiLocationRoib = LVL1::TrigT1CaloDefs::CPMTobRoILocation + "RoIB");
  declareProperty("CPMTowerLocationOverlap",
           m_cpmTowerLocationOverlap =
	                   LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap");

  declareProperty("ForceSlicesCPM", m_forceSlicesCpm = 0);
  declareProperty("ForceSlicesCMX", m_forceSlicesCmx = 0);

  // By default print everything except RoIB RoIs and Tower overlap
  declareProperty("CPMTowerPrint",   m_cpmTowerPrint   = 1);
  declareProperty("CMXCPTobPrint",   m_cmxCpTobPrint   = 1,
                  "Set to 2 to print presence maps (Neutral format only)");
  declareProperty("CMXCPHitsPrint",  m_cmxCpHitsPrint  = 1,
                  "Set to 2 to print overflow bit (Neutral format only)");
  declareProperty("CPMTobRoIPrint",     m_cpmRoiPrint     = 1);
  declareProperty("CPMTobRoIPrintRoIB", m_cpmRoiPrintRoib = 0);
  declareProperty("CPMTowerPrintOverlap", m_cpmTowerPrintOverlap = 0);
}

CpmTesterV2::~CpmTesterV2()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmTesterV2::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  m_towerKey = new LVL1::TriggerTowerKey();

  return StatusCode::SUCCESS;
}

// Execute

StatusCode CpmTesterV2::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  if (m_cpmTowerPrint) {

    // Find CPM towers

    const CpmTowerCollection* ttCollection = 0;
    StatusCode sc = evtStore()->retrieve(ttCollection, m_cpmTowerLocation);
    if (sc.isFailure() || !ttCollection || ttCollection->empty()) {
      msg() << "No core CPM towers found" << endmsg;
    } else {

      // Order by eta, phi

      setupCpmTowerMap(ttCollection);

      // Print the CPM towers

      printCpmTowers("core");
    }
  }

  if (m_cpmTowerPrintOverlap) {

    // Find overlap CPM towers

    const CpmTowerCollection* ttCollection = 0;
    StatusCode sc = evtStore()->retrieve(ttCollection, m_cpmTowerLocationOverlap);
    if (sc.isFailure() || !ttCollection || ttCollection->empty()) {
      msg() << "No overlap CPM towers found" << endmsg;
    } else {

      // Order by eta, phi

      setupCpmTowerMap(ttCollection);

      // Print the overlap CPM towers

      printCpmTowers("overlap");
    }
  }

  if (m_cmxCpTobPrint) {

    // Find CMX-CP TOBs

    const CmxCpTobCollection* tobCollection = 0;
    StatusCode sc = evtStore()->retrieve(tobCollection, m_cmxCpTobLocation);
    if (sc.isFailure() || !tobCollection || tobCollection->empty()) {
      msg() << "No CMX-CP TOBs found" << endmsg;
    } else {

      // Order by crate, cmx, cpm, chip

      setupCmxCpTobMap(tobCollection);

      // Print the CMX-CP TOBs

      printCmxCpTobs();
    }
  }

  if (m_cmxCpHitsPrint) {

    // Find CMX-CP hits

    const CmxCpHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmxCpHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMX-CP Hits found" << endmsg;
    } else {

      // Order by crate, cmx, source

      setupCmxCpHitsMap(hitCollection);

      // Print the CMX-CP hits

      printCmxCpHits();
    }
  }

  if (m_cpmRoiPrint) {

    // Find CPM RoIs

    const CpmRoiCollection* roiCollection = 0;
    StatusCode sc = evtStore()->retrieve(roiCollection, m_cpmRoiLocation);
    if (sc.isFailure() || !roiCollection || roiCollection->empty()) {
      msg() << "No CPM RoIs found" << endmsg;
    } else {

      // Order by RoI word

      setupCpmRoiMap(roiCollection);

      // Print the CPM RoIs

      printCpmRois("DAQ");
    }
  }

  if (m_cpmRoiPrintRoib) {

    // Find CPM RoIs from RoIB

    const CpmRoiCollection* roiCollection = 0;
    StatusCode sc = evtStore()->retrieve(roiCollection, m_cpmRoiLocationRoib);
    if (sc.isFailure() || !roiCollection || roiCollection->empty()) {
      msg() << "No CPM RoIs from RoIB found" << endmsg;
    } else {

      // Order by RoI word

      setupCpmRoiMap(roiCollection);

      // Print the CPM RoIs from RoIB

      printCpmRois("RoIB");
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpmTesterV2::finalize()
{
  delete m_towerKey;

  return StatusCode::SUCCESS;
}

// Print the CPM towers

void CpmTesterV2::printCpmTowers(const std::string& source) const
{
  msg() << "Number of " << source << " CPM towers = "
        << m_ttMap.size() << endmsg;
  CpmTowerMap::const_iterator mapIter = m_ttMap.begin();
  CpmTowerMap::const_iterator mapEnd  = m_ttMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CPMTower* const tt = mapIter->second;
    int peak   = tt->peak();
    int slices = (tt->emEnergyVec()).size();
    if (m_forceSlicesCpm) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCpm);
      slices = m_forceSlicesCpm;
    }
    msg() << "key/eta/phi/peak/em/had/emErr/hadErr: "
          << mapIter->first << "/" << tt->eta() << "/" << tt->phi() << "/"
	  << peak << "/";

    std::vector<int> emEnergy;
    std::vector<int> hadEnergy;
    std::vector<int> emError;
    std::vector<int> hadError;
    ModifySlices::data(tt->emEnergyVec(),  emEnergy,  slices);
    ModifySlices::data(tt->hadEnergyVec(), hadEnergy, slices);
    ModifySlices::data(tt->emErrorVec(),   emError,   slices);
    ModifySlices::data(tt->hadErrorVec(),  hadError,  slices);
    printVec(emEnergy);
    printVec(hadEnergy);
    msg() << MSG::hex;
    printVec(emError);
    printVec(hadError);
    msg() << MSG::dec << endmsg;
  }
}

// Print the CMX-CP TOBs

void CpmTesterV2::printCmxCpTobs() const
{
  msg() << "Number of CMX-CP TOBs = " << m_tobMap.size() << endmsg;
  CmxCpTobMap::const_iterator mapIter = m_tobMap.begin();
  CmxCpTobMap::const_iterator mapEnd  = m_tobMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMXCPTob* const tb = mapIter->second;
    int peak   = tb->peak();
    int slices = (tb->energyVec()).size();
    if (m_forceSlicesCmx) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmx);
      slices = m_forceSlicesCmx;
    }
    if (m_cmxCpTobPrint == 2) {
      msg() << "crate/cmx/cpm/chip/loc/peak/energy/isolation/error/presenceMap: ";
    } else {
      msg() << "crate/cmx/cpm/chip/loc/peak/energy/isolation/error: ";
    }
    msg() << tb->crate() << "/" << tb->cmx() << "/" << tb->cpm() << "/"
	  << tb->chip() << "/" << tb->location() << "/" << peak << "/";

    std::vector<int> energy;
    std::vector<int> isolation;
    std::vector<int> error;
    ModifySlices::data(tb->energyVec(), energy, slices);
    ModifySlices::data(tb->isolationVec(), isolation, slices);
    ModifySlices::data(tb->errorVec(), error, slices);
    printVec(energy);
    msg() << MSG::hex;
    printVec(isolation);
    printVec(error);
    if (m_cmxCpTobPrint == 2) {
      std::vector<unsigned int> presence;
      ModifySlices::data(tb->presenceMapVec(), presence, slices);
      printVecH(presence, 1, 16);
    }
    msg() << MSG::dec << endmsg;
  }
}

// Print the CMX-CP hits

void CpmTesterV2::printCmxCpHits() const
{
  msg() << "Number of CMX-CP Hits = " << m_hitsMap.size() << endmsg;
  CmxCpHitsMap::const_iterator mapIter = m_hitsMap.begin();
  CmxCpHitsMap::const_iterator mapEnd  = m_hitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMXCPHits* const ch = mapIter->second;
    int peak   = ch->peak();
    int slices = (ch->hitsVec0()).size();
    if (m_forceSlicesCmx) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmx);
      slices = m_forceSlicesCmx;
    }
    msg() << "crate/cmx/source/peak/hits0/hits1/err0/err1: "
	  << ch->crate() << "/" << ch->cmx() << "/" << ch->source() << "/"
	  << peak << "/";

    std::vector<unsigned int> hits0;
    std::vector<unsigned int> hits1;
    std::vector<int> err0;
    std::vector<int> err1;
    ModifySlices::data(ch->hitsVec0(), hits0, slices);
    ModifySlices::data(ch->hitsVec1(), hits1, slices);
    ModifySlices::data(ch->errorVec0(), err0, slices);
    ModifySlices::data(ch->errorVec1(), err1, slices);
    int bits = 3;
    int words = 8;
    if (ch->source() == LVL1::CMXCPHits::TOPO_CHECKSUM) {
      bits = 16;
      words = 1;
    } else if (ch->source() == LVL1::CMXCPHits::TOPO_OCCUPANCY_MAP) {
      bits = 1;
      words = 14;
    } else if (ch->source() == LVL1::CMXCPHits::TOPO_OCCUPANCY_COUNTS) {
      words = 7;
    }
    printVecH(hits0, bits, words);
    printVecH(hits1, bits, words);
    msg() << MSG::hex;
    if (m_cmxCpHitsPrint != 2) { // Suppress overflow bit
      for (int sl = 0; sl < slices; ++sl) {
        LVL1::DataError d0(err0[sl]);
	d0.set(LVL1::DataError::Overflow, 0);
	err0[sl] = d0.error();
        LVL1::DataError d1(err1[sl]);
	d1.set(LVL1::DataError::Overflow, 0);
	err1[sl] = d1.error();
      }
    }
    printVec(err0);
    printVec(err1);
    msg() << MSG::dec << endmsg;
  }
}

// Print the CPM RoIs

void CpmTesterV2::printCpmRois(const std::string& source) const
{
  msg() << "Number of CPM RoIs (" << source << ") = " << m_roiMap.size()
        << endmsg;
  CpmRoiMap::const_iterator mapIter = m_roiMap.begin();
  CpmRoiMap::const_iterator mapEnd  = m_roiMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CPMTobRoI* const roi = mapIter->second;
    msg() << "crate/cpm/chip/loc/type/energy/isolation: "
	  << roi->crate()    << "/" << roi->cpm() << "/" << roi->chip() << "/"
	  << roi->location() << "/" << roi->type() << "/"
	  << roi->energy() << "/" << MSG::hex << roi->isolation() << MSG::dec
	  << endmsg;
  }
}

// Print a vector

void CpmTesterV2::printVec(const std::vector<int>& vec) const
{
  std::vector<int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Print a vector of hits

void CpmTesterV2::printVecH(const std::vector<unsigned int>& vec,
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

// Set up CPM tower map

void CpmTesterV2::setupCpmTowerMap(const CpmTowerCollection* const ttCollection)
{
  m_ttMap.clear();
  if (ttCollection) {
    CpmTowerCollection::const_iterator pos  = ttCollection->begin();
    CpmTowerCollection::const_iterator pose = ttCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMTower* const tt = *pos;
      const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
      m_ttMap.insert(std::make_pair(key, tt));
    }
  }
}

// Set up CMX-CP TOB map

void CpmTesterV2::setupCmxCpTobMap(const CmxCpTobCollection* const tobCollection)
{ 
  m_tobMap.clear();
  if (tobCollection) { 
    CmxCpTobCollection::const_iterator pos  = tobCollection->begin();
    CmxCpTobCollection::const_iterator pose = tobCollection->end();
    for (; pos != pose; ++pos) {
      LVL1::CMXCPTob* const tob = *pos;
      const int crate = tob->crate();
      const int cmx = tob->cmx(); 
      const int cpm = tob->cpm();
      const int chip = tob->chip();
      const int loc = tob->location();
      const int key = (((((((crate<<1)|cmx)<<4)|cpm)<<4)|chip)<<2)|loc;
      m_tobMap.insert(std::make_pair(key, tob));
    }
  }
}

// Set up CMX-CP hits map

void CpmTesterV2::setupCmxCpHitsMap(const CmxCpHitsCollection*
                                                       const hitCollection)
{
  m_hitsMap.clear();
  if (hitCollection) {
    CmxCpHitsCollection::const_iterator pos  = hitCollection->begin();
    CmxCpHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMXCPHits* const hits = *pos;
      const int key = (((hits->crate()<<1)|hits->cmx())<<3)|hits->source();
      m_hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CPM RoI map

void CpmTesterV2::setupCpmRoiMap(const CpmRoiCollection* const roiCollection)
{
  m_roiMap.clear();
  if (roiCollection) {
    CpmRoiCollection::const_iterator pos  = roiCollection->begin();
    CpmRoiCollection::const_iterator pose = roiCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMTobRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

} // end namespace
