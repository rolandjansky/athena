/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <utility>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "CpmTester.h"
#include "../src/ModifySlices.h"

namespace LVL1BS {

CpmTester::CpmTester(const std::string& name, ISvcLocator* pSvcLocator)
                     : AthAlgorithm(name, pSvcLocator),
		       m_towerKey(0)
{
  declareProperty("CPMTowerLocation",
           m_cpmTowerLocation   = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMHitsLocation",
           m_cpmHitsLocation    = LVL1::TrigT1CaloDefs::CPMHitsLocation);
  declareProperty("CMMCPHitsLocation",
           m_cmmCpHitsLocation  = LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
  declareProperty("CPMRoILocation",
           m_cpmRoiLocation     = LVL1::TrigT1CaloDefs::CPMRoILocation);
  declareProperty("CPMRoILocationRoIB",
           m_cpmRoiLocationRoib =
	                   LVL1::TrigT1CaloDefs::CPMRoILocation + "RoIB");
  declareProperty("CPMTowerLocationOverlap",
           m_cpmTowerLocationOverlap =
	                   LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap");

  declareProperty("ForceSlicesCPM", m_forceSlicesCpm = 0);
  declareProperty("ForceSlicesCMM", m_forceSlicesCmm = 0);

  // By default print everything except RoIB RoIs and Tower overlap
  declareProperty("CPMTowerPrint",   m_cpmTowerPrint   = 1);
  declareProperty("CPMHitsPrint",    m_cpmHitsPrint    = 1);
  declareProperty("CMMCPHitsPrint",  m_cmmCpHitsPrint  = 1);
  declareProperty("CPMRoIPrint",     m_cpmRoiPrint     = 1);
  declareProperty("CPMRoIPrintRoIB", m_cpmRoiPrintRoib = 0);
  declareProperty("CPMTowerPrintOverlap", m_cpmTowerPrintOverlap = 0);
}

CpmTester::~CpmTester()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  m_towerKey = new LVL1::TriggerTowerKey();

  return StatusCode::SUCCESS;
}

// Execute

StatusCode CpmTester::execute()
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

  if (m_cpmHitsPrint) {

    // Find CPM hits

    const CpmHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cpmHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CPM Hits found" << endmsg;
    } else {

      // Order by crate, module

      setupCpmHitsMap(hitCollection);

      // Print the CPM hits

      printCpmHits();
    }
  }

  if (m_cmmCpHitsPrint) {

    // Find CMM-CP hits

    const CmmCpHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmmCpHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMM-CP Hits found" << endmsg;
    } else {

      // Order by crate, dataID

      setupCmmCpHitsMap(hitCollection);

      // Print the CMM-CP hits

      printCmmCpHits();
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

StatusCode CpmTester::finalize()
{
  delete m_towerKey;

  return StatusCode::SUCCESS;
}

// Print the CPM towers

void CpmTester::printCpmTowers(const std::string& source) const
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

// Print the CPM hits

void CpmTester::printCpmHits() const
{
  msg() << "Number of CPM Hits = " << m_hitsMap.size() << endmsg;
  CpmHitsMap::const_iterator mapIter = m_hitsMap.begin();
  CpmHitsMap::const_iterator mapEnd  = m_hitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CPMHits* const ch = mapIter->second;
    int peak   = ch->peak();
    int slices = (ch->HitsVec0()).size();
    if (m_forceSlicesCpm) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCpm);
      slices = m_forceSlicesCpm;
    }
    msg() << "crate/module/peak/hits0/hits1: "
	  << ch->crate() << "/" << ch->module() << "/" << peak << "/";

    std::vector<unsigned int> hits0;
    std::vector<unsigned int> hits1;
    ModifySlices::data(ch->HitsVec0(), hits0, slices);
    ModifySlices::data(ch->HitsVec1(), hits1, slices);
    printVecH(hits0);
    printVecH(hits1);
    msg() << endmsg;
  }
}

// Print the CMM-CP hits

void CpmTester::printCmmCpHits() const
{
  msg() << "Number of CMM-CP Hits = " << m_cmmHitsMap.size() << endmsg;
  CmmCpHitsMap::const_iterator mapIter = m_cmmHitsMap.begin();
  CmmCpHitsMap::const_iterator mapEnd  = m_cmmHitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMMCPHits* const ch = mapIter->second;
    int peak   = ch->peak();
    int slices = (ch->HitsVec0()).size();
    if (m_forceSlicesCmm) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmm);
      slices = m_forceSlicesCmm;
    }
    msg() << "crate/dataID/peak/hits0/hits1/err0/err1: "
	  << ch->crate() << "/" << ch->dataID() << "/" << peak << "/";

    std::vector<unsigned int> hits0;
    std::vector<unsigned int> hits1;
    std::vector<int> err0;
    std::vector<int> err1;
    ModifySlices::data(ch->HitsVec0(), hits0, slices);
    ModifySlices::data(ch->HitsVec1(), hits1, slices);
    ModifySlices::data(ch->ErrorVec0(), err0, slices);
    ModifySlices::data(ch->ErrorVec1(), err1, slices);
    printVecH(hits0);
    printVecH(hits1);
    msg() << MSG::hex;
    printVec(err0);
    printVec(err1);
    msg() << MSG::dec << endmsg;
  }
}

// Print the CPM RoIs

void CpmTester::printCpmRois(const std::string& source) const
{
  msg() << "Number of CPM RoIs (" << source << ") = " << m_roiMap.size()
        << endmsg;
  CpmRoiMap::const_iterator mapIter = m_roiMap.begin();
  CpmRoiMap::const_iterator mapEnd  = m_roiMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CPMRoI* const roi = mapIter->second;
    msg() << "crate/cpm/chip/loc/hits/error: "
	  << roi->crate()    << "/" << roi->cpm() << "/" << roi->chip() << "/"
	  << roi->location() << "/";
    int hits = roi->hits();
    for (int i = 0; i < 16; ++i) {
      if (i == 8)     msg() << ";";
      else if (i > 0) msg() << ":";
      msg() << ((hits >> i) & 0x1);
    }
    msg() << "/" << roi->error() << "/" << endmsg;
  }
}

// Print a vector

void CpmTester::printVec(const std::vector<int>& vec) const
{
  std::vector<int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Print a vector of hits

void CpmTester::printVecH(const std::vector<unsigned int>& vec) const
{
  const int words = 8;
  const int bits  = 3;
  const unsigned int mask = 0x7;
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

void CpmTester::setupCpmTowerMap(const CpmTowerCollection* const ttCollection)
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

// Set up CPM hits map

void CpmTester::setupCpmHitsMap(const CpmHitsCollection* const hitCollection)
{
  m_hitsMap.clear();
  if (hitCollection) {
    CpmHitsCollection::const_iterator pos  = hitCollection->begin();
    CpmHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMHits* const hits = *pos;
      const int key = hits->crate()*100 + hits->module() - 1;
      m_hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM-CP hits map

void CpmTester::setupCmmCpHitsMap(const CmmCpHitsCollection*
                                                       const hitCollection)
{
  m_cmmHitsMap.clear();
  if (hitCollection) {
    CmmCpHitsCollection::const_iterator pos  = hitCollection->begin();
    CmmCpHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMCPHits* const hits = *pos;
      const int key = hits->crate()*100 + hits->dataID();
      m_cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CPM RoI map

void CpmTester::setupCpmRoiMap(const CpmRoiCollection* const roiCollection)
{
  m_roiMap.clear();
  if (roiCollection) {
    CpmRoiCollection::const_iterator pos  = roiCollection->begin();
    CpmRoiCollection::const_iterator pose = roiCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CPMRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

} // end namespace
