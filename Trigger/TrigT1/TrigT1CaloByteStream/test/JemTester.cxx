/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "JemTester.h"
#include "../src/ModifySlices.h"

namespace LVL1BS {

JemTester::JemTester(const std::string& name, ISvcLocator* pSvcLocator)
                     : AthAlgorithm(name, pSvcLocator),
		       m_elementKey(0)
{
  declareProperty("JetElementLocation",
           m_jetElementLocation = LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JEMHitsLocation",
           m_jemHitsLocation    = LVL1::TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("JEMEtSumsLocation",
           m_jemEtSumsLocation  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMMJetHitsLocation",
           m_cmmJetLocation     = LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
  declareProperty("CMMEtSumsLocation",
           m_cmmEnergyLocation  = LVL1::TrigT1CaloDefs::CMMEtSumsLocation);
  declareProperty("JEMRoILocation",
           m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMRoILocation);
  declareProperty("CMMRoILocation",
           m_cmmRoiLocation     = LVL1::TrigT1CaloDefs::CMMRoILocation);
  declareProperty("JEMRoILocationRoIB",
           m_jemRoiLocationRoib =
	                 LVL1::TrigT1CaloDefs::JEMRoILocation + "RoIB");
  declareProperty("CMMRoILocationRoIB",
           m_cmmRoiLocationRoib =
	                 LVL1::TrigT1CaloDefs::CMMRoILocation + "RoIB");
  declareProperty("JetElementLocationOverlap",
           m_jetElementLocationOverlap =
	                 LVL1::TrigT1CaloDefs::JetElementLocation + "Overlap");

  declareProperty("ForceSlicesJEM",  m_forceSlicesJem  = 0);
  declareProperty("ForceSlicesCMM",  m_forceSlicesCmm  = 0);

  // By default print everything except RoIB RoIs and jet element overlap
  declareProperty("JetElementPrint", m_jetElementPrint = 1);
  declareProperty("JEMHitsPrint",    m_jemHitsPrint    = 1);
  declareProperty("JEMEtSumsPrint",  m_jemEtSumsPrint  = 1);
  declareProperty("CMMJetHitsPrint", m_cmmHitsPrint    = 1);
  declareProperty("CMMEtSumsPrint",  m_cmmEtSumsPrint  = 1);
  declareProperty("JEMRoIPrint",     m_jemRoiPrint     = 1);
  declareProperty("CMMRoIPrint",     m_cmmRoiPrint     = 1);
  declareProperty("JEMRoIPrintRoIB", m_jemRoiPrintRoib = 0);
  declareProperty("CMMRoIPrintRoIB", m_cmmRoiPrintRoib = 0);
  declareProperty("JetElementPrintOverlap", m_jetElementPrintOverlap = 0);
}

JemTester::~JemTester()
{
}

// Initialize

StatusCode JemTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  m_elementKey = new LVL1::JetElementKey();

  return StatusCode::SUCCESS;
}

// Execute

StatusCode JemTester::execute()
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

  if (m_jemHitsPrint) {

    // Find jet hits

    const JetHitsCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_jemHitsLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No Jet Hits found" << endmsg;
    } else {

      // Order by crate, module

      setupHitsMap(hitCollection);

      // Print the jet hits

      printJetHits();
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

  if (m_cmmHitsPrint) {

    // Find CMM hits

    const CmmJetCollection* hitCollection = 0;
    StatusCode sc = evtStore()->retrieve(hitCollection, m_cmmJetLocation);
    if (sc.isFailure() || !hitCollection || hitCollection->empty()) {
      msg() << "No CMM Hits found" << endmsg;
    } else {

      // Order by crate, dataID

      setupCmmHitsMap(hitCollection);

      // Print the CMM hits

      printCmmHits();
    }
  }

  if (m_cmmEtSumsPrint) {

    // Find CMM energy sums

    const CmmEnergyCollection* etCollection = 0;
    StatusCode sc = evtStore()->retrieve(etCollection, m_cmmEnergyLocation);
    if (sc.isFailure() || !etCollection || etCollection->empty()) {
      msg() << "No CMM Energy Sums found" << endmsg;
    } else {

      // Order by crate, dataID

      setupCmmEtMap(etCollection);

      // Print the CMM energy sums

      printCmmSums();
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

  if (m_cmmRoiPrint) {

    // Find CMM RoIs

    const LVL1::CMMRoI* crCollection = 0;
    StatusCode sc = evtStore()->retrieve(crCollection, m_cmmRoiLocation);
    if (sc.isFailure() || !crCollection || (!crCollection->jetEtRoiWord()   &&
                                            !crCollection->energyRoiWord0() &&
					    !crCollection->energyRoiWord1() &&
					    !crCollection->energyRoiWord2())) {
      msg() << "No CMM RoIs found" << endmsg;
    } else {

      // Print the CMM RoIs

      printCmmRois("DAQ", crCollection);
    }
  }

  if (m_cmmRoiPrintRoib) {

    // Find CMM RoIs from RoIB

    const LVL1::CMMRoI* crCollection = 0;
    StatusCode sc = evtStore()->retrieve(crCollection, m_cmmRoiLocationRoib);
    if (sc.isFailure() || !crCollection || (!crCollection->jetEtRoiWord()   &&
                                            !crCollection->energyRoiWord0() &&
					    !crCollection->energyRoiWord1() &&
					    !crCollection->energyRoiWord2())) {
      msg() << "No CMM RoIs from RoIB found" << endmsg;
    } else {

      // Print the CMM RoIs from RoIB

      printCmmRois("RoIB", crCollection);
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode JemTester::finalize()
{
  delete m_elementKey;

  return StatusCode::SUCCESS;
}

// Print the jet elements

void JemTester::printJetElements(const std::string& source) const
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
    msg() << "key/eta/phi/peak/em/had/emErr/hadErr/linkErr: "
          << mapIter->first << "/" << je->eta() << "/" << je->phi() << "/"
	  << peak << "/";

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
    printVec(emEnergy);
    printVec(hadEnergy);
    msg() << MSG::hex;
    printVec(emError);
    printVec(hadError);
    printVec(linkError);
    msg() << MSG::dec << endmsg;
  }
}

// Print the jet hits

void JemTester::printJetHits() const
{
  msg() << "Number of Jet Hits = " << m_hitsMap.size() << endmsg;
  JetHitsMap::const_iterator mapIter = m_hitsMap.begin();
  JetHitsMap::const_iterator mapEnd  = m_hitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::JEMHits* const jh = mapIter->second;
    int peak   = jh->peak();
    int slices = (jh->JetHitsVec()).size();
    if (m_forceSlicesJem) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesJem);
      slices = m_forceSlicesJem;
    }
    msg() << "crate/module/peak/hits: "
	  << jh->crate() << "/" << jh->module() << "/" << peak << "/";
    int words = 8;
    int bits  = 3;
    unsigned int mask = 0x7;
    if (jh->forward()) {
      words = 12;
      bits  = 2;
      mask  = 0x3;
    }
    std::vector<unsigned int> jetHits;
    ModifySlices::data(jh->JetHitsVec(), jetHits, slices);
    std::vector<unsigned int>::const_iterator pos;
    std::vector<unsigned int>::const_iterator posb = jetHits.begin();
    std::vector<unsigned int>::const_iterator pose = jetHits.end();
    for (pos = posb; pos != pose; ++pos) {
      if (pos != posb) msg() << ",";
      const unsigned int hits = *pos;
      for (int i = 0; i < words; ++i) {
        if (i != 0) msg() << ":";
	const unsigned int thr = (hits >> bits*i) & mask;
        msg() << thr;
      }
    }
    msg() << "/" << endmsg;
  }
}

// Print energy sums

void JemTester::printEnergySums() const
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

// Print the CMM hits

void JemTester::printCmmHits() const
{
  msg() << "Number of CMM Hits = " << m_cmmHitsMap.size() << endmsg;
  CmmHitsMap::const_iterator mapIter = m_cmmHitsMap.begin();
  CmmHitsMap::const_iterator mapEnd  = m_cmmHitsMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMMJetHits* const jh = mapIter->second;
    const int dataID = jh->dataID();
    int peak   = jh->peak();
    int slices = (jh->HitsVec()).size();
    if (m_forceSlicesCmm) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmm);
      slices = m_forceSlicesCmm;
    }
    msg() << "crate/dataID/peak/hits/error: "
	  << jh->crate() << "/" << dataID << "/" << peak << "/";
    int words = 8;
    int bits  = 3;
    unsigned int mask = 0x7;
    if (dataID == 0 || dataID == 7 || dataID == 8 || dataID == 15) {
      words = 12;
      bits  = 2;
      mask  = 0x3;
    } else if (dataID == LVL1::CMMJetHits::REMOTE_FORWARD ||
               dataID == LVL1::CMMJetHits::LOCAL_FORWARD  ||
	       dataID == LVL1::CMMJetHits::TOTAL_FORWARD) {
      bits  = 2;
      mask  = 0x3;
    } else if (dataID == LVL1::CMMJetHits::ET_MAP) {
      words = 1;
      bits  = 4;
      mask  = 0xf;
    }
    std::vector<unsigned int> hitsVec;
    ModifySlices::data(jh->HitsVec(), hitsVec, slices);
    std::vector<unsigned int>::const_iterator pos;
    std::vector<unsigned int>::const_iterator posb = hitsVec.begin();
    std::vector<unsigned int>::const_iterator pose = hitsVec.end();
    for (pos = posb; pos != pose; ++pos) {
      if (pos != posb) msg() << ",";
      const unsigned int hits = *pos;
      for (int i = 0; i < words; ++i) {
        if (i != 0) msg() << ":";
	const unsigned int thr = (hits >> bits*i) & mask;
        msg() << thr;
      }
    }
    msg() << "/" << MSG::hex;
    std::vector<int> errorVec;
    ModifySlices::data(jh->ErrorVec(), errorVec, slices);
    printVec(errorVec);
    msg() << MSG::dec << endmsg;
  }
}

// Print CMM energy sums

void JemTester::printCmmSums() const
{
  msg() << "Number of CMM Energy Sums = " << m_cmmEtMap.size() << endmsg;
  CmmSumsMap::const_iterator mapIter = m_cmmEtMap.begin();
  CmmSumsMap::const_iterator mapEnd  = m_cmmEtMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::CMMEtSums* const et = mapIter->second;
    int peak   = et->peak();
    int slices = (et->ExVec()).size();
    if (m_forceSlicesCmm) {
      peak   = ModifySlices::peak(peak, slices, m_forceSlicesCmm);
      slices = m_forceSlicesCmm;
    }
    msg() << "crate/dataID/peak/Ex/Ey/Et/ExErr/EyErr/EtErr: "
	  << et->crate() << "/" << et->dataID() << "/" << peak << "/";
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

void JemTester::printJemRois(const std::string& source) const
{
  msg() << "Number of JEM RoIs (" << source << ") = " << m_roiMap.size()
        << endmsg;
  JemRoiMap::const_iterator mapIter = m_roiMap.begin();
  JemRoiMap::const_iterator mapEnd  = m_roiMap.end();
  for (; mapIter != mapEnd; ++mapIter) {
    const LVL1::JEMRoI* const roi = mapIter->second;
    msg() << "crate/jem/frame/loc/fwd/hits/error: "
          << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	  << roi->location() << "/" << roi->forward() << "/";
    const int hits = roi->hits();
    const int numHits = (roi->forward()) ? 4 : 8;
    for (int i = 0; i < numHits; ++i) {
      if (i > 0) msg() << ":";
      msg() << ((hits >> i) & 0x1);
    }
    msg() << "/" << roi->error() << "/" << endmsg;
  }
}

// Print the CMM RoIs

void JemTester::printCmmRois(const std::string& source,
                             const LVL1::CMMRoI* const roi) const
{
  msg() << "CMMRoI(" << source
        << "):jetEtHits/sumEtHits/missingEtHits/missingEtSigHits/Ex/Ey/Et;error: "
        << MSG::hex << roi->jetEtHits() << ";" << roi->jetEtError() << "/"
        << roi->sumEtHits() << ";" << roi->sumEtError() << "/"
        << roi->missingEtHits() << ";" << roi->missingEtError() << "/"
        << roi->missingEtSigHits() << ";" << roi->missingEtSigError() << "/"
        << MSG::dec << roi->ex() << ";" << roi->exError() << "/"
        << roi->ey() << ";" << roi->eyError() << "/"
        << roi->et() << ";" << roi->etError() << "/" << endmsg;
}

// Print a vector

void JemTester::printVec(const std::vector<int>& vec) const
{
  std::vector<int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Print a vector (unsigned)

void JemTester::printVecU(const std::vector<unsigned int>& vec) const
{
  std::vector<unsigned int>::const_iterator pos;
  for (pos = vec.begin(); pos != vec.end(); ++pos) {
    if (pos != vec.begin()) msg() << ",";
    msg() << *pos;
  }
  msg() << "/";
}

// Set up jet element map

void JemTester::setupJeMap(const JetElementCollection* const jeCollection)
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

// Set up jet hits map

void JemTester::setupHitsMap(const JetHitsCollection* const hitCollection)
{
  m_hitsMap.clear();
  if (hitCollection) {
    JetHitsCollection::const_iterator pos  = hitCollection->begin();
    JetHitsCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMHits* const hits = *pos;
      const int key = hits->crate()*100 + hits->module();
      m_hitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up energy sums map

void JemTester::setupEtMap(const EnergySumsCollection* const etCollection)
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

// Set up CMM hits map

void JemTester::setupCmmHitsMap(const CmmJetCollection* const hitCollection)
{
  m_cmmHitsMap.clear();
  if (hitCollection) {
    CmmJetCollection::const_iterator pos  = hitCollection->begin();
    CmmJetCollection::const_iterator pose = hitCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMJetHits* const hits = *pos;
      const int key = hits->crate()*100 + hits->dataID();
      m_cmmHitsMap.insert(std::make_pair(key, hits));
    }
  }
}

// Set up CMM energy sums map

void JemTester::setupCmmEtMap(const CmmEnergyCollection* const etCollection)
{
  m_cmmEtMap.clear();
  if (etCollection) {
    CmmEnergyCollection::const_iterator pos  = etCollection->begin();
    CmmEnergyCollection::const_iterator pose = etCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::CMMEtSums* const sums = *pos;
      if (std::accumulate((sums->ExVec()).begin(), (sums->ExVec()).end(), 0) ||
          std::accumulate((sums->EyVec()).begin(), (sums->EyVec()).end(), 0) ||
          std::accumulate((sums->EtVec()).begin(), (sums->EtVec()).end(), 0) ||
          std::accumulate((sums->ExErrorVec()).begin(),
	                                      (sums->ExErrorVec()).end(), 0) ||
          std::accumulate((sums->EyErrorVec()).begin(),
	                                      (sums->EyErrorVec()).end(), 0) ||
          std::accumulate((sums->EtErrorVec()).begin(),
	                                      (sums->EtErrorVec()).end(), 0)) {
        const int key = sums->crate()*100 + sums->dataID();
        m_cmmEtMap.insert(std::make_pair(key, sums));
      }
    }
  }
}

// Set up JEM RoI map

void JemTester::setupJemRoiMap(const JemRoiCollection* const jrCollection)
{
  m_roiMap.clear();
  if (jrCollection) {
    JemRoiCollection::const_iterator pos  = jrCollection->begin();
    JemRoiCollection::const_iterator pose = jrCollection->end();
    for (; pos != pose; ++pos) {
      const LVL1::JEMRoI* const roi = *pos;
      const uint32_t key = roi->roiWord();
      m_roiMap.insert(std::make_pair(key, roi));
    }
  }
}

} // end namespace
