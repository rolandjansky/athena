/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <utility>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "../src/ModifySlices.h"

#include "PpmTester.h"

namespace LVL1BS {

PpmTester::PpmTester(const std::string& name, ISvcLocator* pSvcLocator)
                     : AthAlgorithm(name, pSvcLocator),
		       m_towerKey(0)
{
  declareProperty("TriggerTowerLocation",
         m_triggerTowerLocation = LVL1::TrigT1CaloDefs::TriggerTowerLocation);

  declareProperty("ForceSlicesLUT",  m_forceSlicesLut  = 0);
  declareProperty("ForceSlicesFADC", m_forceSlicesFadc = 0);
}

PpmTester::~PpmTester()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode PpmTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endmsg;

  m_towerKey = new LVL1::TriggerTowerKey();

  return StatusCode::SUCCESS;
}

// Execute

StatusCode PpmTester::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  // Find trigger towers

  const TriggerTowerCollection* ttCollection = 0;
  StatusCode sc = evtStore()->retrieve(ttCollection, m_triggerTowerLocation);
  if (sc.isFailure() || !ttCollection || ttCollection->empty()) {
    msg() << "No Trigger Towers found" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Order by key

  setupTTMap(ttCollection);

  // Print the trigger towers

  printTriggerTowers();

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode PpmTester::finalize()
{
  delete m_towerKey;

  return StatusCode::SUCCESS;
}

// Print the trigger towers

void PpmTester::printTriggerTowers() const
{
  msg() << "Number of Trigger Towers = " << m_ttMap.size() << endmsg;
  TriggerTowerMap::const_iterator mapIter = m_ttMap.begin();
  TriggerTowerMap::const_iterator mapEnd  = m_ttMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::TriggerTower* const tt = mapIter->second;
    int emPeak       = tt->emPeak();
    int hadPeak      = tt->hadPeak();
    int emADCPeak    = tt->emADCPeak();
    int hadADCPeak   = tt->hadADCPeak();
    int emLUTSlices  = (tt->emLUT()).size();
    int emADCSlices  = (tt->emADC()).size();
    int hadLUTSlices = (tt->hadLUT()).size();
    int hadADCSlices = (tt->hadADC()).size();
    if (m_forceSlicesLut) {
      emPeak  = ModifySlices::peak(emPeak, emLUTSlices, m_forceSlicesLut);
      hadPeak = ModifySlices::peak(hadPeak, hadLUTSlices, m_forceSlicesLut);
      emLUTSlices  = m_forceSlicesLut;
      hadLUTSlices = m_forceSlicesLut;
    }
    if (m_forceSlicesFadc) {
      emADCPeak  = ModifySlices::peak(emADCPeak, emADCSlices,
                                                            m_forceSlicesFadc);
      hadADCPeak = ModifySlices::peak(hadADCPeak, hadADCSlices,
                                                            m_forceSlicesFadc);
      emADCSlices  = m_forceSlicesFadc;
      hadADCSlices = m_forceSlicesFadc;
    }
    msg() << " EM:key/eta/phi/LUTpeak/FADCpeak/LUT/FADC/bcidLUT/bcidFADC/error: "
          << mapIter->first << "/" << tt->eta() << "/" << tt->phi() << "/"
	  << emPeak << "/" << emADCPeak << "/";
    std::vector<int> lut;
    std::vector<int> fadc;
    std::vector<int> bcidLut;
    std::vector<int> bcidFadc;
    ModifySlices::data(tt->emLUT(),     lut,      emLUTSlices);
    ModifySlices::data(tt->emADC(),     fadc,     emADCSlices);
    ModifySlices::data(tt->emBCIDvec(), bcidLut,  emLUTSlices);
    ModifySlices::data(tt->emBCIDext(), bcidFadc, emADCSlices);
    printVec(lut);
    printVec(fadc);
    printVec(bcidLut);
    printVec(bcidFadc);
    msg() << MSG::hex << tt->emError() << MSG::dec << "/" << endmsg;
    msg() << "HAD:key/eta/phi/LUTpeak/FADCpeak/LUT/FADC/bcidLUT/bcidFADC/error: "
          << mapIter->first << "/" << tt->eta() << "/" << tt->phi() << "/"
	  << hadPeak << "/" << hadADCPeak << "/";
    ModifySlices::data(tt->hadLUT(),     lut,      hadLUTSlices);
    ModifySlices::data(tt->hadADC(),     fadc,     hadADCSlices);
    ModifySlices::data(tt->hadBCIDvec(), bcidLut,  hadLUTSlices);
    ModifySlices::data(tt->hadBCIDext(), bcidFadc, hadADCSlices);
    printVec(lut);
    printVec(fadc);
    printVec(bcidLut);
    printVec(bcidFadc);
    msg() << MSG::hex << tt->hadError() << MSG::dec << "/" << endmsg;
  }
}

// Print a vector

void PpmTester::printVec(const std::vector<int>& vec) const
{
  std::vector<int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Set up trigger tower map

void PpmTester::setupTTMap(const TriggerTowerCollection* const ttCollection)
{
  m_ttMap.clear();
  TriggerTowerCollection::const_iterator pos  = ttCollection->begin();
  TriggerTowerCollection::const_iterator pose = ttCollection->end();
  for (; pos != pose; ++pos) {
    const LVL1::TriggerTower* const tt = *pos;
    // doesn't work for spare channels (unphysical eta/phi):
    //const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
    const unsigned int key = tt->key();
    m_ttMap.insert(std::make_pair(key, tt));
  }
}

} // end namespace
