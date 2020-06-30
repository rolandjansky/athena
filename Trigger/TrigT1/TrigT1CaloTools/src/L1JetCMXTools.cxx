/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <map>
#include <numeric>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"

#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "L1JetCMXTools.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

namespace LVL1 {

/** Constructor */

L1JetCMXTools::L1JetCMXTools(const std::string &type, const std::string &name,
                             const IInterface *parent)
    : AthAlgTool(type, name, parent),
      m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name), m_crates(2),
      m_modules(16), m_maxTobs(4), m_sysCrate(1), m_debug(false) {
  declareInterface<IL1JetCMXTools>(this);
  declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

/** Destructor */

L1JetCMXTools::~L1JetCMXTools() {}

/** Initialisation */

StatusCode L1JetCMXTools::initialize() {
  m_debug = msgLvl(MSG::DEBUG);

  // Connect to the LVL1ConfigSvc for the trigger configuration:

  StatusCode sc = m_configSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endmsg;
    return sc;
  } else if (m_debug) {
    msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName() << endmsg;
  }

  msg(MSG::INFO) << "Initialization completed" << endmsg;

  return sc;
}

/** Finalisation */

StatusCode L1JetCMXTools::finalize() { return StatusCode::SUCCESS; }

/** form CMX-Jet TOBs from RoIs - single slice */

void L1JetCMXTools::formCMXJetTob(const xAOD::JEMTobRoIContainer *jemRoiVec,
                                  xAOD::CMXJetTobContainer *cmxTobVec) const {
  std::vector<const xAOD::JEMTobRoIContainer *> jemRoiColls(1, jemRoiVec);
  formCMXJetTob(jemRoiColls, cmxTobVec, 0);
}

void L1JetCMXTools::formCMXJetTob(
    const std::vector<const xAOD::JEMTobRoIContainer *> &jemRoiColls,
    xAOD::CMXJetTobContainer *cmxTobVec, int peak) const {
  std::map<uint32_t, const xAOD::JEMTobRoI *> jemRoiMap;
  std::map<int, xAOD::CMXJetTob *> cmxTobMap;
  xAOD::JEMTobRoIContainer::const_iterator it;
  int timeslices = jemRoiColls.size();
  for (int slice = 0; slice < timeslices; ++slice) {
    const xAOD::JEMTobRoIContainer *jemRoiVec = jemRoiColls[slice];
    jemRoiMap.clear();
    std::vector<unsigned int> presenceMaps(m_crates * m_modules);
    std::vector<int> tobCount(m_crates * m_modules);
    xAOD::JEMTobRoIContainer::const_iterator it = jemRoiVec->begin();
    xAOD::JEMTobRoIContainer::const_iterator itE = jemRoiVec->end();
    for (; it != itE; ++it) { // get sorted list
      const xAOD::JEMTobRoI *roi = *it;
      const int crate = roi->crate();
      const int jem = roi->jem();
      const int index = crate * m_modules + jem;
      const int presenceBit = roi->frame(); // <<== CHECK
      presenceMaps[index] |= (1 << presenceBit);
      tobCount[index]++;
      uint32_t key = roi->roiWord();
      jemRoiMap.insert(std::make_pair(key, roi));
    }
    std::map<uint32_t, const xAOD::JEMTobRoI *>::const_iterator mit =
        jemRoiMap.begin();
    std::map<uint32_t, const xAOD::JEMTobRoI *>::const_iterator mitE =
        jemRoiMap.end();
    for (; mit != mitE; ++mit) {
      const xAOD::JEMTobRoI *roi = mit->second;
      const int crate = roi->crate();
      const int jem = roi->jem();
      const int frame = roi->frame();
      const int loc = roi->location();
      const int index = crate * m_modules + jem;
      const int energyLg = roi->energyLarge();
      const int energySm = roi->energySmall();
      const unsigned int presence = presenceMaps[index];
      int error = 0;
      if (tobCount[index] > m_maxTobs) { // overflow
        int count = 0;
        for (int bit = 0; bit <= frame; ++bit)
          count += (presence >> bit) & 0x1;
        if (count > m_maxTobs)
          continue;
        LVL1::DataError err;
        err.set(LVL1::DataError::Overflow);
        error = err.error();
      }
      const int key = (((((crate << 4) | jem) << 3) | frame) << 2) | loc;
      xAOD::CMXJetTob *tob = 0;
      std::map<int, xAOD::CMXJetTob *>::iterator xit = cmxTobMap.find(key);
      if (xit == cmxTobMap.end()) {
        tob = new xAOD::CMXJetTob;
        tob->makePrivateStore();
        tob->initialize(crate, jem, frame, loc);
        if (timeslices > 0) { // TODO(amazurov): need to check >1 or >0
          std::vector<uint16_t> vecU16(timeslices);
          std::vector<uint32_t> vecU32(timeslices);
          tob->addTob(vecU16, vecU16, vecU32, vecU16);
          tob->setPeak(peak);
        }
        cmxTobMap.insert(std::make_pair(key, tob));
        cmxTobVec->push_back(tob);
      } else
        tob = xit->second;
      std::vector<uint16_t> energyLgVec(tob->energyLargeVec());
      std::vector<uint16_t> energySmVec(tob->energySmallVec());
      std::vector<uint32_t> errorVec(tob->errorVec());
      std::vector<uint16_t> presenceMapVec(tob->presenceMapVec());
      energyLgVec[slice] = energyLg;
      energySmVec[slice] = energySm;
      errorVec[slice] = error;
      presenceMapVec[slice] = presence;
      tob->addTob(energyLgVec, energySmVec, errorVec, presenceMapVec);
    }
  }
}

void L1JetCMXTools::formCMXJetHits(
    const xAOD::CMXJetTobContainer *cmxTobVec,
    xAOD::CMXJetHitsContainer *cmxHitsVec) const {
  xAOD::CMXJetHitsContainer *cmxHitsCrate = new xAOD::CMXJetHitsContainer;
  xAOD::CMXJetHitsContainer *cmxHitsSys = new xAOD::CMXJetHitsContainer;
  xAOD::CMXJetHitsContainer *cmxHitsTopo = new xAOD::CMXJetHitsContainer;

  formCMXJetHitsCrate(cmxTobVec, cmxHitsCrate);
  formCMXJetHitsSystem(cmxHitsCrate, cmxHitsSys);
  formCMXJetHitsTopo(cmxTobVec, cmxHitsTopo);

  mergeCMXJetHits(cmxHitsVec, cmxHitsCrate);
  mergeCMXJetHits(cmxHitsVec, cmxHitsSys);
  mergeCMXJetHits(cmxHitsVec, cmxHitsTopo);

  delete cmxHitsCrate;
  delete cmxHitsSys;
  delete cmxHitsTopo;
}

void L1JetCMXTools::formCMXJetHitsCrate(
    const xAOD::CMXJetTobContainer *cmxTobVec,
    xAOD::CMXJetHitsContainer *cmxHitsCrate) const {
  int peakm = 0;
  std::vector<HitsVector> hitVecM(2 * m_crates);
  std::vector<HitsVector> hitVecF(2 * m_crates);
  std::vector<ErrorVector> errVecM(2 * m_crates); // Need overflow
  std::vector<ErrorVector> errVecF(2 * m_crates);
  HitsVector hit10, hit20;
  HitsVector hit11, hit21;
  xAOD::CMXJetTobContainer::const_iterator pos = cmxTobVec->begin();
  xAOD::CMXJetTobContainer::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const xAOD::CMXJetTob *tob = *pos;
    const int index = 2 * tob->crate();
    const std::vector<uint32_t> error(tob->errorVec());
    hit10.clear();
    hit11.clear();
    hit20.clear();
    hit21.clear();
    getHits(tob, hit10, hit11, hit20, hit21);

    addCMXJetHits(hitVecM[index], hit10, MAIN_HITS);
    addCMXJetHits(hitVecM[index + 1], hit11, MAIN_HITS);
    addCMXJetHits(hitVecF[index], hit20, FORWARD_HITS);
    addCMXJetHits(hitVecF[index + 1], hit21, FORWARD_HITS);

    addOverflow(errVecF[index], error);
    addOverflow(errVecF[index + 1], error);
    addOverflow(errVecM[index], error);
    addOverflow(errVecM[index + 1], error);
    const int peak = tob->peak();
    if (peak > peakm)
      peakm = peak;
  }
  // Save non-zero crate totals
  for (int crate = 0; crate < m_crates; ++crate) {
    const int index = crate * 2;
    saveCMXJetHits(cmxHitsCrate, hitVecM[index], hitVecM[index + 1],
                   errVecM[index], errVecM[index + 1], crate,
                   xAOD::CMXJetHits::LOCAL_MAIN, peakm);
    saveCMXJetHits(cmxHitsCrate, hitVecF[index], hitVecF[index + 1],
                   errVecF[index], errVecF[index + 1], crate,
                   xAOD::CMXJetHits::LOCAL_FORWARD, peakm);
    if (crate != m_sysCrate) { // REMOTE totals
      saveCMXJetHits(cmxHitsCrate, hitVecM[index], hitVecM[index + 1],
                     errVecM[index], errVecM[index + 1], m_sysCrate,
                     xAOD::CMXJetHits::REMOTE_MAIN, peakm);
      saveCMXJetHits(cmxHitsCrate, hitVecF[index], hitVecF[index + 1],
                     errVecF[index], errVecF[index + 1], m_sysCrate,
                     xAOD::CMXJetHits::REMOTE_FORWARD, peakm);
    }
  }
}

void L1JetCMXTools::formCMXJetHitsSystem(
    const xAOD::CMXJetHitsContainer *cmxHitsCrate,
    xAOD::CMXJetHitsContainer *cmxHitsSys) const {
  int peakm = 0;
  HitsVector systemMain0(1);
  HitsVector systemMain1(1);
  HitsVector systemFwd0(1);
  HitsVector systemFwd1(1);
  ErrorVector errVec(1);
  xAOD::CMXJetHitsContainer::const_iterator pos = cmxHitsCrate->begin();
  xAOD::CMXJetHitsContainer::const_iterator pose = cmxHitsCrate->end();
  for (; pos != pose; ++pos) {
    const xAOD::CMXJetHits *hits = *pos;
    if (hits->crate() != m_sysCrate)
      continue;
    int source = hits->sourceComponent();
    if (source != xAOD::CMXJetHits::LOCAL_MAIN &&
        source != xAOD::CMXJetHits::LOCAL_FORWARD &&
        source != xAOD::CMXJetHits::REMOTE_MAIN &&
        source != xAOD::CMXJetHits::REMOTE_FORWARD)
      continue;
    int peak = hits->peak();
    if (peak > peakm)
      peakm = peak;
    HitsVector hitsVec0(hits->hitsVec0());
    HitsVector hitsVec1(hits->hitsVec1());
    if (source == xAOD::CMXJetHits::LOCAL_MAIN ||
        source == xAOD::CMXJetHits::REMOTE_MAIN) {
      addCMXJetHits(systemMain0, hitsVec0, MAIN_HITS);
      addCMXJetHits(systemMain1, hitsVec1, MAIN_HITS);
    } else {
      addCMXJetHits(systemFwd0, hitsVec0, FORWARD_HITS);
      addCMXJetHits(systemFwd1, hitsVec1, FORWARD_HITS);
    }
    ErrorVector error(hits->errorVec0()); // all have same error so redundant?
    addOverflow(errVec, error);
  }
  // Save non-zero system totals
  saveCMXJetHits(cmxHitsSys, systemMain0, systemMain1, errVec, errVec,
                 m_sysCrate, CMXJetHits::TOTAL_MAIN, peakm);
  saveCMXJetHits(cmxHitsSys, systemFwd0, systemFwd1, errVec, errVec, m_sysCrate,
                 CMXJetHits::TOTAL_FORWARD, peakm);
}

void L1JetCMXTools::formCMXJetHitsTopo(
    const xAOD::CMXJetTobContainer *cmxTobVec,
    xAOD::CMXJetHitsContainer *cmxHitsTopo) const {
  int peakm = 0;
  int timeslices = 0;
  std::vector<HitsVector> hitVec(4 * m_crates);
  xAOD::CMXJetTobContainer::const_iterator pos = cmxTobVec->begin();
  xAOD::CMXJetTobContainer::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const xAOD::CMXJetTob *tob = *pos;
    const int crate = tob->crate();
    const int jem = tob->jem();
    const int frame = tob->frame();
    const int loc = tob->location();
    const int index = crate * 4;
    const std::vector<int> energyLg(tob->energyLargeVec().begin(),
                                    tob->energyLargeVec().end());
    const std::vector<int> energySm(tob->energySmallVec().begin(),
                                    tob->energySmallVec().end());
    // const std::vector<int>& error(tob->errorVec());
    timeslices = energyLg.size();
    HitsVector &checksum(hitVec[index]);
    HitsVector &map(hitVec[index + 1]);
    HitsVector &countsLow(hitVec[index + 2]);
    HitsVector &countsHigh(hitVec[index + 3]);
    checksum.resize(timeslices);
    map.resize(timeslices);
    countsLow.resize(timeslices);
    countsHigh.resize(timeslices);
    for (int slice = 0; slice < timeslices; ++slice) {
      if (energyLg[slice] == 0 && energySm[slice] == 0)
        continue;
      // checksum
      // LVL1::DataError err(error[slice]);
      // const int overflow = err.get(LVL1::DataError::Overflow);
      const int overflow = 0; // don't include overflow as not in slink data
      checksum[slice] +=
          jem + frame + loc + energyLg[slice] + energySm[slice] + overflow;
      checksum[slice] &= 0xffff;
      // occupancy map
      map[slice] |= (1 << jem);
      // occupancy counts
      if (jem < 8) {
        countsLow[slice] += (1 << (3 * jem)); // can't saturate
      } else {
        countsHigh[slice] += (1 << (3 * (jem - 8)));
      }
    }
    const int peak = tob->peak();
    if (peak > peakm)
      peakm = peak;
  }
  // Save non-zero crate totals
  HitsVector dummy(timeslices);
  ErrorVector dummyE(timeslices);
  for (int crate = 0; crate < m_crates; ++crate) {
    const int index = crate * 4;
    saveCMXJetHits(cmxHitsTopo, hitVec[index], dummy, dummyE, dummyE, crate,
                   xAOD::CMXJetHits::TOPO_CHECKSUM, peakm);
    saveCMXJetHits(cmxHitsTopo, hitVec[index + 1], dummy, dummyE, dummyE, crate,
                   xAOD::CMXJetHits::TOPO_OCCUPANCY_MAP, peakm);
    saveCMXJetHits(cmxHitsTopo, hitVec[index + 2], hitVec[index + 3], dummyE,
                   dummyE, crate, xAOD::CMXJetHits::TOPO_OCCUPANCY_COUNTS,
                   peakm);
  }
}

/** Temporary for testing, mostly lifted from JetAlgorithm */

void L1JetCMXTools::getHits(const xAOD::CMXJetTob *tob, HitsVector &hit10,
                            HitsVector &hit11, HitsVector &hit20,
                            HitsVector &hit21) const {
  using namespace TrigConf;
  const std::vector<uint16_t> &energyLg(tob->energyLargeVec());
  const std::vector<uint16_t> &energySm(tob->energySmallVec());
  const int timeslices = energyLg.size();
  auto err = LVL1::DataError(tob->error());

  hit10.assign(timeslices, 0);
  hit11.assign(timeslices, 0);
  hit20.assign(timeslices, 0);
  hit21.assign(timeslices, 0);

  std::vector<TrigConf::TriggerThreshold *> thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
  for (int slice = 0; slice < timeslices; ++slice) {
    if (err.get(LVL1::DataError::Overflow)) {
      hit10[slice] = 0x7fff;
      hit11[slice] = 0x7fff;
      hit20[slice] = 0xffff;
      hit21[slice] = 0x3fff;
      continue;
    }

    if (energyLg[slice] == 0 && energySm[slice] == 0)
      continue;

    xAOD::JEMTobRoI tobRoi;
    tobRoi.makePrivateStore();
    tobRoi.initialize(tob->crate(), tob->jem(), tob->frame(), tob->location(),
                      energyLg[slice], energySm[slice]);
    LVL1::RecJetRoI roi(tobRoi.roiWord(), &thresholds);

    int numThresholdsHalf = 5;
    int numBitsPerCounter = 3;

    for (int i = 0; i < numThresholdsHalf * 2; ++i) {
      if (roi.passedThreshold(i)) {
        HitsVector &hit = i < numThresholdsHalf ? hit10 : hit11;
        int ibit = i < numThresholdsHalf ? i : i - numThresholdsHalf;
        hit[slice] |= (1 << (ibit * numBitsPerCounter));
      }
    }

    numThresholdsHalf = 8;
    numBitsPerCounter = 2;

    for (int i = 0; i < numThresholdsHalf * 2; ++i) {
      if (roi.passedThreshold(10 + i)) {
        HitsVector &hit = i < numThresholdsHalf ? hit20 : hit21;
        int ibit = i < numThresholdsHalf ? i : i - numThresholdsHalf;
        hit[slice] |= (1 << (ibit * numBitsPerCounter));
      }
    }

  } // end slice for-loop
}

/** Add overflow bit */

void L1JetCMXTools::addOverflow(ErrorVector &hitErr,
                                const ErrorVector &tobErr) const {
  const int timeslices = tobErr.size();
  hitErr.resize(timeslices);
  for (int slice = 0; slice < timeslices; ++slice) {
    int error = tobErr[slice];
    if (error) {
      LVL1::DataError err(error);
      int overflow = err.get(LVL1::DataError::Overflow);
      LVL1::DataError err2;
      err2.set(LVL1::DataError::Overflow, overflow);
      hitErr[slice] |= err2.error();
    }
  }
}

/** Add hits from second vector to first */

void L1JetCMXTools::addCMXJetHits(HitsVector &vec1, const HitsVector &vec2,
                                  HitsType type) const {
  int size1 = vec1.size();
  int size2 = vec2.size();
  if (size1 < size2)
    vec1.resize(size2);
  HitsVector::iterator pos1 = vec1.begin();
  HitsVector::iterator pose1 = vec1.end();
  HitsVector::const_iterator pos2 = vec2.begin();
  HitsVector::const_iterator pose2 = vec2.end();
  for (; pos1 != pose1 && pos2 != pose2; ++pos1, ++pos2) {
    if (type == MAIN_HITS)
      *pos1 = addHits(*pos1, *pos2, 15, 15, 5);
    else
      *pos1 = addHits(*pos1, *pos2, 16, 16, 8);
  }
}

/** Increment JEM/CMX hit word */

unsigned int L1JetCMXTools::addHits(unsigned int hitMult, unsigned int hitVec,
                                    int multBits, int vecBits,
                                    int nthresh) const {
  if (m_debug)
    msg(MSG::DEBUG) << "addHits: Original hitMult = " << std::hex << hitMult
                    << ". Add hitWord = " << hitVec << std::dec << endmsg;

  int nbitsOut = multBits / nthresh;
  int nbitsIn = vecBits / nthresh;

  if (m_debug)
    msg(MSG::DEBUG) << " Bits per threshold = " << nbitsOut << endmsg;

  int max = (1 << nbitsOut) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = (1 << nbitsIn) - 1;
  unsigned int shift = 0;

  unsigned int hits = 0;

  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult & multMask) + (hitVec & hitMask);
    mult = ((mult <= max) ? mult : max);
    hits += (mult << shift);

    hitMult >>= nbitsOut;
    hitVec >>= nbitsIn;
    shift += nbitsOut;
  }

  if (m_debug)
    msg(MSG::DEBUG) << "addHits returning hitMult = " << std::hex << hits
                    << std::dec << endmsg;

  return hits;
}

/** Merge CMX-Jet hits vectors */

void L1JetCMXTools::mergeCMXJetHits(
    xAOD::CMXJetHitsContainer *cmxHitsVec1,
    xAOD::CMXJetHitsContainer *cmxHitsVec2) const {
  int size = cmxHitsVec2->size();
  for (int index = 0; index < size; ++index) {
    xAOD::CMXJetHits *hitsIn = 0;
    xAOD::CMXJetHits *hitsOut = 0;
    cmxHitsVec2->swapElement(index, hitsIn, hitsOut);
    cmxHitsVec1->push_back(hitsOut);
  }
  cmxHitsVec2->clear();
}

/** Save non-zero CMX-Jet hits */

void L1JetCMXTools::saveCMXJetHits(xAOD::CMXJetHitsContainer *cmxHitsVec,
                                   const HitsVector &hits0,
                                   const HitsVector &hits1,
                                   const ErrorVector &err0,
                                   const ErrorVector &err1, int crate,
                                   int source, int peak) const {
  if (std::accumulate(hits0.begin(), hits0.end(), 0) ||
      std::accumulate(hits1.begin(), hits1.end(), 0)) {
    xAOD::CMXJetHits *item = new xAOD::CMXJetHits;
    item->makePrivateStore();
    item->initialize(crate, source, hits0, hits1, err0, err1, peak);
    cmxHitsVec->push_back(item);
  }
}

} // end of namespace
