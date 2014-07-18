/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <sstream>
#include <utility>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloSim/MergeSlices.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

namespace LVL1 {


MergeSlices::MergeSlices(const std::string& name, ISvcLocator* pSvcLocator)
                       : Algorithm(name, pSvcLocator),
		         m_storeGate("StoreGateSvc", name)
{
  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty("TimeslicesLUT", m_slicesLut = 1,
                  "The number of timeslices to be merged");

  declareProperty("CPMHitsLocation",
         m_cpmHitsLocation      = TrigT1CaloDefs::CPMHitsLocation);
  declareProperty("CPMTowerLocation",
         m_cpmTowerLocation     = TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("JEMEtSumsLocation",
         m_jemEtSumsLocation    = TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("JEMHitsLocation",
         m_jemHitsLocation      = TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("JetElementLocation",
         m_jetElementLocation   = TrigT1CaloDefs::JetElementLocation);
  declareProperty("TriggerTowerLocation",
         m_triggerTowerLocation = TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("FullADCLocation",
         m_fullAdcLocation      = TrigT1CaloDefs::TriggerTowerLocation+"ADC");
}

MergeSlices::~MergeSlices()
{
}

// Initialize

StatusCode MergeSlices::initialize()
{
  MsgStream log( msgSvc(), name() );

  StatusCode sc = m_storeGate.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName()
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName()
        << endreq;
  }
  return StatusCode::SUCCESS;
}

// Execute

StatusCode MergeSlices::execute()
{
  MsgStream log( msgSvc(), name() );

  // PPM

  StatusCode sc = mergePpm();
  if (sc.isFailure()) {
    log << MSG::ERROR << "PPM merge failed" << endreq;
  }

  // CPM

  sc = mergeCpm();
  if (sc.isFailure()) {
    log << MSG::ERROR << "CPM merge failed" << endreq;
  }

  // JEM

  sc = mergeJem();
  if (sc.isFailure()) {
    log << MSG::ERROR << "JEM merge failed" << endreq;
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode MergeSlices::finalize()
{

  return StatusCode::SUCCESS;
}

// Merge PPM containers

StatusCode MergeSlices::mergePpm()
{
  MsgStream log( msgSvc(), name() );

  // Create map for output TriggerTowers

  TriggerTowerMap ttMap;
  
  // Find full ADC digits

  const TriggerTowerCollection* adcCollection = 0;
  StatusCode sc = m_storeGate->retrieve(adcCollection, m_fullAdcLocation);
  if (sc.isFailure() || !adcCollection) {
    log << MSG::ERROR << "No ADC digits container found" << endreq;
    return sc;
  }
  
  // Copy and put in map

  TriggerTowerCollection::const_iterator pos  = adcCollection->begin();
  TriggerTowerCollection::const_iterator pose = adcCollection->end();
  std::vector<int> dummyLut(m_slicesLut);
  for (; pos != pose; ++pos) {
    // Have to copy these because constructor doesn't take const
    std::vector<int> emADC((*pos)->emADC());
    std::vector<int> hadADC((*pos)->hadADC());
    std::vector<int> emBCIDext((*pos)->emBCIDext());
    std::vector<int> hadBCIDext((*pos)->hadBCIDext());
    TriggerTower* tt = new TriggerTower(
                       (*pos)->phi(), (*pos)->eta(), (*pos)->key(),
		       emADC, dummyLut, emBCIDext, dummyLut,
	               (*pos)->emError(), m_slicesLut/2, (*pos)->emADCPeak(),
                       hadADC, dummyLut, hadBCIDext, dummyLut,
	               (*pos)->hadError(), m_slicesLut/2, (*pos)->hadADCPeak());
    ttMap.insert(std::make_pair((*pos)->key(), tt));
  }

  // Merge in LUT data for each slice

  for (int slice = 0; slice < m_slicesLut; ++slice) {
    std::string location = m_triggerTowerLocation + numString(slice);
    const TriggerTowerCollection* ttColl = 0;
    StatusCode sc = m_storeGate->retrieve(ttColl, location);
    if (sc.isFailure() || !ttColl) {
      log << MSG::ERROR << "No TriggerTower container found for slice "
          << slice << " key " << location << endreq;
      continue;
    }
    pos  = ttColl->begin();
    pose = ttColl->end();
    for (; pos != pose; ++pos) {
      TriggerTowerMap::iterator adcPos = ttMap.find((*pos)->key());
      if (adcPos == ttMap.end()) {
        log << MSG::ERROR << "Missing ADC data for TT key " << (*pos)->key()
	    << endreq;
        continue;
      }
      TriggerTower* tt = adcPos->second;
      std::vector<int> emADC(tt->emADC());
      std::vector<int> hadADC(tt->hadADC());
      std::vector<int> emLUT(tt->emLUT());
      std::vector<int> hadLUT(tt->hadLUT());
      std::vector<int> emBCIDext(tt->emBCIDext());
      std::vector<int> hadBCIDext(tt->hadBCIDext());
      std::vector<int> emBCIDvec(tt->emBCIDvec());
      std::vector<int> hadBCIDvec(tt->hadBCIDvec());
      emLUT[slice]      = (*pos)->emEnergy();
      hadLUT[slice]     = (*pos)->hadEnergy();
      emBCIDvec[slice]  = (*pos)->emBCID();
      hadBCIDvec[slice] = (*pos)->hadBCID();
      int emADCPeak     = tt->emADCPeak();
      int hadADCPeak    = tt->hadADCPeak();
      int emLUTPeak     = tt->emPeak();
      int hadLUTPeak    = tt->hadPeak();
      int emError       = tt->emError();
      int hadError      = tt->hadError();
      tt->addEM(emADC, emLUT, emBCIDext, emBCIDvec,
                       emError, emLUTPeak, emADCPeak);
      tt->addHad(hadADC, hadLUT, hadBCIDext, hadBCIDvec,
                         hadError, hadLUTPeak, hadADCPeak);
    }
  }

  // Fill container and save

  TriggerTowerCollection* ttOut = new TriggerTowerCollection;
  TriggerTowerMap::iterator mapIter;
  TriggerTowerMap::iterator mapIterEnd;
  mapIter    = ttMap.begin();
  mapIterEnd = ttMap.end();
  for (; mapIter != mapIterEnd; ++mapIter) ttOut->push_back(mapIter->second);
  sc = m_storeGate->overwrite(ttOut, m_triggerTowerLocation,true,false,false);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Error registering trigger tower collection in TDS"
        << endreq;
    delete ttOut;
  }

  return sc;
}

// Merge CPM containers

StatusCode MergeSlices::mergeCpm()
{
  MsgStream log( msgSvc(), name() );

  // Create maps and key provider for output CPM Towers and Hits

  CPMTowerMap towerMap;
  CPMHitsMap  hitsMap;
  TriggerTowerKey towerKey;

  // Merge in data for each slice

  for (int slice = 0; slice < m_slicesLut; ++slice) {
  
    // Find CPM towers for this slice

    const CPMTowerCollection* ttCollection = 0;
    std::string location = m_cpmTowerLocation + numString(slice);
    StatusCode sc = m_storeGate->retrieve(ttCollection, location);
    if (sc.isFailure() || !ttCollection || ttCollection->empty()) {
      log << MSG::DEBUG << "No CPM Towers found for slice " << slice << endreq;
    } else {
      CPMTowerCollection::const_iterator pos  = ttCollection->begin();
      CPMTowerCollection::const_iterator pose = ttCollection->end();
      for (; pos != pose; ++pos) {
        CPMTower* tt = *pos;
	CPMTower* ttOut = 0;
	unsigned int key = towerKey.ttKey(tt->phi(), tt->eta());
        CPMTowerMap::iterator mPos = towerMap.find(key);
	if (mPos == towerMap.end()) {
	  std::vector<int> vec(m_slicesLut);
	  ttOut = new CPMTower(tt->phi(), tt->eta(),
	                       vec, vec, vec, vec, m_slicesLut/2);
	  towerMap.insert(std::make_pair(key, ttOut));
        } else ttOut = mPos->second;
	std::vector<int> emEnergy(ttOut->emEnergyVec());
	std::vector<int> hadEnergy(ttOut->hadEnergyVec());
	std::vector<int> emError(ttOut->emErrorVec());
	std::vector<int> hadError(ttOut->hadErrorVec());
	emEnergy[slice]  = tt->emEnergy();
	hadEnergy[slice] = tt->hadEnergy();
	emError[slice]   = tt->emError();
	hadError[slice]  = tt->hadError();
	ttOut->fill(emEnergy, emError, hadEnergy, hadError, m_slicesLut/2);
      }
    }

    // Find CPM hits for this slice

    const CPMHitsCollection* hitCollection = 0;
    location = m_cpmHitsLocation + numString(slice);
    sc = m_storeGate->retrieve(hitCollection, location);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      log << MSG::DEBUG << "No CPM Hits found for slice " << slice << endreq;
    } else {
      CPMHitsCollection::const_iterator pos  = hitCollection->begin();
      CPMHitsCollection::const_iterator pose = hitCollection->end();
      for (; pos != pose; ++pos) {
        CPMHits* hits = *pos;
	CPMHits* hitsOut = 0;
	int key = hits->crate() * 100 + hits->module();
	CPMHitsMap::iterator mPos = hitsMap.find(key);
	if (mPos == hitsMap.end()) {
	  std::vector<unsigned int> vec(m_slicesLut);
	  hitsOut = new CPMHits(hits->crate(), hits->module(),
	                        vec, vec, m_slicesLut/2);
          hitsMap.insert(std::make_pair(key, hitsOut));
        } else hitsOut = mPos->second;
	std::vector<unsigned int> hits0(hitsOut->HitsVec0());
	std::vector<unsigned int> hits1(hitsOut->HitsVec1());
	hits0[slice] = hits->HitWord0();
	hits1[slice] = hits->HitWord1();
	hitsOut->addHits(hits0, hits1);
      }
    }
  }

  // Fill containers and save

  CPMTowerCollection* cpmtOut = new CPMTowerCollection;
  CPMTowerMap::iterator mapIter;
  CPMTowerMap::iterator mapIterEnd;
  mapIter    = towerMap.begin();
  mapIterEnd = towerMap.end();
  for (; mapIter != mapIterEnd; ++mapIter) cpmtOut->push_back(mapIter->second);
  StatusCode sc1 = m_storeGate->overwrite(cpmtOut, m_cpmTowerLocation,true,false,false);
  if (sc1.isFailure()) {
    log << MSG::ERROR << "Error registering CPM Tower collection in TDS"
        << endreq;
    delete cpmtOut;
  }
  CPMHitsCollection* cpmhOut = new CPMHitsCollection;
  CPMHitsMap::iterator hitIter;
  CPMHitsMap::iterator hitIterEnd;
  hitIter    = hitsMap.begin();
  hitIterEnd = hitsMap.end();
  for (; hitIter != hitIterEnd; ++hitIter) cpmhOut->push_back(hitIter->second);
  StatusCode sc2 = m_storeGate->overwrite(cpmhOut, m_cpmHitsLocation,true,false,false);
  if (sc2.isFailure()) {
    log << MSG::ERROR << "Error registering CPM Hits collection in TDS"
        << endreq;
    delete cpmhOut;
  }

  if (sc1.isFailure() || sc2.isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

// Merge JEM containers

StatusCode MergeSlices::mergeJem()
{
  MsgStream log( msgSvc(), name() );

  // Create maps and key provider for output JetElements, JEMHits and JEMEtSums

  JetElementMap elementMap;
  JEMHitsMap    hitsMap;
  JEMEtSumsMap  sumsMap;
  JetElementKey elementKey;

  // Merge in data for each slice

  for (int slice = 0; slice < m_slicesLut; ++slice) {
  
    // Find Jet Elements for this slice

    const JetElementCollection* jeCollection = 0;
    std::string location = m_jetElementLocation + numString(slice);
    StatusCode sc = m_storeGate->retrieve(jeCollection, location);
    if (sc.isFailure() || !jeCollection || jeCollection->empty()) {
      log << MSG::DEBUG << "No Jet Elements found for slice " << slice
          << endreq;
    } else {
      JetElementCollection::const_iterator pos  = jeCollection->begin();
      JetElementCollection::const_iterator pose = jeCollection->end();
      for (; pos != pose; ++pos) {
        JetElement* je = *pos;
	JetElement* jeOut = 0;
	unsigned int key = elementKey.jeKey(je->phi(), je->eta());
        JetElementMap::iterator mPos = elementMap.find(key);
	if (mPos == elementMap.end()) {
	  std::vector<int> vec(m_slicesLut);
	  jeOut = new JetElement(je->phi(), je->eta(),
 	                         vec, vec, key, vec, vec, vec, m_slicesLut/2);
	  elementMap.insert(std::make_pair(key, jeOut));
        } else jeOut = mPos->second;
	jeOut->addSlice(slice, je->emEnergy(), je->hadEnergy(),
	                je->emError(), je->hadError(), je->linkError());
      }
    }
  
    // Find JEM Hits for this slice

    const JEMHitsCollection* jhCollection = 0;
    location = m_jemHitsLocation + numString(slice);
    sc = m_storeGate->retrieve(jhCollection, location);
    if (sc.isFailure() || !jhCollection || jhCollection->empty()) {
      log << MSG::DEBUG << "No JEM Hits found for slice " << slice
          << endreq;
    } else {
      JEMHitsCollection::const_iterator pos  = jhCollection->begin();
      JEMHitsCollection::const_iterator pose = jhCollection->end();
      for (; pos != pose; ++pos) {
        JEMHits* jh = *pos;
	JEMHits* jhOut = 0;
	int key = jh->crate() * 100 + jh->module();
	JEMHitsMap::iterator mPos = hitsMap.find(key);
	if (mPos == hitsMap.end()) {
	  std::vector<unsigned int> vec(m_slicesLut);
	  jhOut = new JEMHits(jh->crate(), jh->module(), vec, m_slicesLut/2);
	  hitsMap.insert(std::make_pair(key, jhOut));
        } else jhOut = mPos->second;
	std::vector<unsigned int> jetHits(jhOut->JetHitsVec());
	jetHits[slice] = jh->JetHits();
	jhOut->addJetHits(jetHits);
      }
    }
  
    // Find JEM EtSums for this slice

    const JEMEtSumsCollection* jsCollection = 0;
    location = m_jemEtSumsLocation + numString(slice);
    sc = m_storeGate->retrieve(jsCollection, location);
    if (sc.isFailure() || !jsCollection || jsCollection->empty()) {
      log << MSG::DEBUG << "No JEM EtSums found for slice " << slice
          << endreq;
    } else {
      JEMEtSumsCollection::const_iterator pos  = jsCollection->begin();
      JEMEtSumsCollection::const_iterator pose = jsCollection->end();
      for (; pos != pose; ++pos) {
        JEMEtSums* js = *pos;
	JEMEtSums* jsOut = 0;
	int key = js->crate() * 100 + js->module();
	JEMEtSumsMap::iterator mPos = sumsMap.find(key);
	if (mPos == sumsMap.end()) {
	  std::vector<unsigned int> vec(m_slicesLut);
	  jsOut = new JEMEtSums(js->crate(), js->module(),
	                        vec, vec, vec, m_slicesLut/2);
	  sumsMap.insert(std::make_pair(key, jsOut));
        } else jsOut = mPos->second;
	std::vector<unsigned int> ex(jsOut->ExVec());
	std::vector<unsigned int> ey(jsOut->EyVec());
	std::vector<unsigned int> et(jsOut->EtVec());
	ex[slice] = js->Ex();
	ey[slice] = js->Ey();
	et[slice] = js->Et();
	jsOut->addEx(ex);
	jsOut->addEy(ey);
	jsOut->addEt(et);
      }
    }
  }

  // Fill containers and save

  JetElementCollection* jeteOut = new JetElementCollection;
  JetElementMap::iterator mapIter;
  JetElementMap::iterator mapIterEnd;
  mapIter    = elementMap.begin();
  mapIterEnd = elementMap.end();
  for (; mapIter != mapIterEnd; ++mapIter) jeteOut->push_back(mapIter->second);
  StatusCode sc1 = m_storeGate->overwrite(jeteOut, m_jetElementLocation,true,false,false);
  if (sc1.isFailure()) {
    log << MSG::ERROR << "Error registering Jet Element collection in TDS"
        << endreq;
    delete jeteOut;
  }
  JEMHitsCollection* jemhOut = new JEMHitsCollection;
  JEMHitsMap::iterator hitIter;
  JEMHitsMap::iterator hitIterEnd;
  hitIter    = hitsMap.begin();
  hitIterEnd = hitsMap.end();
  for (; hitIter != hitIterEnd; ++hitIter) jemhOut->push_back(hitIter->second);
  StatusCode sc2 = m_storeGate->overwrite(jemhOut, m_jemHitsLocation,true,false,false);
  if (sc2.isFailure()) {
    log << MSG::ERROR << "Error registering JEM Hits collection in TDS"
        << endreq;
    delete jemhOut;
  }
  JEMEtSumsCollection* jemeOut = new JEMEtSumsCollection;
  JEMEtSumsMap::iterator sumIter;
  JEMEtSumsMap::iterator sumIterEnd;
  sumIter    = sumsMap.begin();
  sumIterEnd = sumsMap.end();
  for (; sumIter != sumIterEnd; ++sumIter) jemeOut->push_back(sumIter->second);
  StatusCode sc3 = m_storeGate->overwrite(jemeOut, m_jemEtSumsLocation,true,false,false);
  if (sc3.isFailure()) {
    log << MSG::ERROR << "Error registering JEM Et Sums collection in TDS"
        << endreq;
    delete jemeOut;
  }

  if (sc1.isFailure() || sc2.isFailure() || sc3.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// Return number as a string

std::string MergeSlices::numString(int num)
{
  std::ostringstream cnum;
  cnum << num;
  return cnum.str();
}

} // end namespace
