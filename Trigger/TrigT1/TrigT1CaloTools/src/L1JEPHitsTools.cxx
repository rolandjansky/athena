/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JEPHitsTools.cxx,  
///////////////////////////////////////////////////////////////////

#include <map>
#include <numeric>
#include <algorithm>
#include <sstream>

#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"

#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "L1JEPHitsTools.h"

namespace LVL1 {

/** Constructor */

L1JEPHitsTools::L1JEPHitsTools(const std::string& type,
                               const std::string& name,
                               const IInterface*  parent)
  :
  AthAlgTool(type, name, parent),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
{
  declareInterface<IL1JEPHitsTools>(this);
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

/** Destructor */

L1JEPHitsTools::~L1JEPHitsTools()
{       
}


/** Initialisation */

StatusCode L1JEPHitsTools::initialize()
{
  m_debug = msgLvl(MSG::DEBUG);


  // Connect to the LVL1ConfigSvc for the trigger configuration:

  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't connect to " << m_configSvc.typeAndName() );
    return sc;
  }
  ATH_MSG_DEBUG( "Connected to " << m_configSvc.typeAndName() );

  
  ATH_MSG_INFO( "Initialization completed" );
  
  return sc;
}

/** Finalisation */

StatusCode L1JEPHitsTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** JetAlgorithm to JEMRoI conversion */

void L1JEPHitsTools::formJEMRoI(const DataVector<JetAlgorithm>* jetAlgorithmVec,
                                      DataVector<JEMRoI>*       jemRoiVec) const
{
  DataVector<JetAlgorithm>::const_iterator pos  = jetAlgorithmVec->begin();
  DataVector<JetAlgorithm>::const_iterator pose = jetAlgorithmVec->end();
  for (; pos != pose; ++pos) {
    JEMRoI* roi = new JEMRoI((*pos)->RoIWord());
    jemRoiVec->push_back(roi);
  }
}

/** JetROI to JEMRoI conversion */

void L1JEPHitsTools::formJEMRoI(const DataVector<JetROI>* jetRoiVec,
                                      DataVector<JEMRoI>* jemRoiVec) const
{
  DataVector<JetROI>::const_iterator pos  = jetRoiVec->begin();
  DataVector<JetROI>::const_iterator pose = jetRoiVec->end();
  for (; pos != pose; ++pos) {
    JEMRoI* roi = new JEMRoI((*pos)->roiWord());
    jemRoiVec->push_back(roi);
  }
}

/** form JEM hits from RoIs - single slice */

void L1JEPHitsTools::formJEMHits(const DataVector<JEMRoI>*  jemRoiVec,
                                       DataVector<JEMHits>* jemHitsVec) const
{
  std::vector<const DataVector<JEMRoI>*> jemRoiColls(1, jemRoiVec);
  formJEMHits(jemRoiColls, jemHitsVec, 0);
}

/** form JEM hits from RoIs - multiple slices */

void L1JEPHitsTools::formJEMHits(
                     const std::vector<const DataVector<JEMRoI>*>& jemRoiColls,
		     DataVector<JEMHits>* jemHitsVec, int peak) const
{
  JetEnergyModuleKey testKey;
  std::map<unsigned int, JEMHits*> jemHitsMap;
  DataVector<JEMRoI>::const_iterator it;
  int timeslices = jemRoiColls.size();

  for (int slice = 0; slice < timeslices; ++slice) {
    const DataVector<JEMRoI>* jemRoiVec = jemRoiColls[slice];
  
    for (it=jemRoiVec->begin(); it!=jemRoiVec->end(); ++it){
      const JEMRoI* roi = *it;
      unsigned int crate = roi->crate();
      unsigned int module = roi->jem();
      unsigned int key = testKey.jemKey(crate, module);
      if (m_debug) {
        JEPRoIDecoder decoder;
        CoordinateRange coord = decoder.coordinate(roi->roiWord());
	if (timeslices > 1) ATH_MSG_DEBUG( "Slice " << slice << ", ");
        ATH_MSG_DEBUG( "Found RoI with (eta, phi) = ("
              << coord.eta() << ", " << coord.phi() << ") " << ", RoIWord = "
              << std::hex << roi->roiWord() << std::dec );
        ATH_MSG_DEBUG( "Crate = " << crate << ", Module = " << module
              << ", JEM key = " << key );
      }
      JEMHits* jemHits=0;
      // find whether corresponding JEMHits already exists
      std::map<unsigned int, JEMHits*>::iterator test=jemHitsMap.find(key);
      // if not, create it
      if ( test==jemHitsMap.end()){
        ATH_MSG_DEBUG( "New key. JEM has crate = " 
                        << crate << ", Module = " << module );
        ATH_MSG_DEBUG( "Create new JEMHits" ); 
        jemHits = new JEMHits(crate, module);
	if (timeslices > 1) {
	  HitsVector hitVec(timeslices);
	  jemHits->addJetHits(hitVec);
	  jemHits->setPeak(peak);
        }
      
        ATH_MSG_DEBUG( "and insert into map" ); 
        jemHitsMap.insert(std::map<unsigned int,JEMHits*>::value_type(key,jemHits));
        jemHitsVec->push_back(jemHits);
      }
      else {
        ATH_MSG_DEBUG( "Existing JEMHits" ); 
        jemHits = test->second; // Already exists, so set pointer
      }
      // increment hit multiplicity.
      ATH_MSG_DEBUG( "Update JEM hits" ); 
      HitsVector hitvec(jemHits->JetHitsVec());
      unsigned int hits = hitvec[slice];
      int nthresh = TrigT1CaloDefs::numOfJetThresholds;
      if (jemHits->forward()) nthresh += TrigT1CaloDefs::numOfFwdJetThresholds;
      hits = addHits(hits, roi->roiWord()&0xFFF, 24, nthresh, nthresh);
      hitvec[slice] = hits;
      jemHits->addJetHits(hitvec);
      ATH_MSG_DEBUG( "All done for this one" ); 
    }
  }
}

/** form complete CMM-Jet hits from JEM hits */

void L1JEPHitsTools::formCMMJetHits(const DataVector<JEMHits>* jemHitsVec,
                                    DataVector<CMMJetHits>* cmmHitsVec) const
{
  DataVector<CMMJetHits>* cmmHitsCrate = new DataVector<CMMJetHits>;
  DataVector<CMMJetHits>* cmmHitsSys   = new DataVector<CMMJetHits>;
  DataVector<CMMJetHits>* cmmHitsMap   = new DataVector<CMMJetHits>;
  formCMMJetHitsModule(jemHitsVec, cmmHitsVec);
  formCMMJetHitsCrate(cmmHitsVec, cmmHitsCrate);
  formCMMJetHitsSystem(cmmHitsCrate, cmmHitsSys);
  formCMMJetHitsEtMap(cmmHitsSys, cmmHitsMap);
  mergeCMMJetHits(cmmHitsVec, cmmHitsCrate);
  mergeCMMJetHits(cmmHitsVec, cmmHitsSys);
  mergeCMMJetHits(cmmHitsVec, cmmHitsMap);
  delete cmmHitsCrate;
  delete cmmHitsSys;
  delete cmmHitsMap;
}

/** form partial CMM-Jet hits (module) from JEM hits */

void L1JEPHitsTools::formCMMJetHitsModule(const DataVector<JEMHits>* jemHitsVec,
                                      DataVector<CMMJetHits>* cmmHitsMod) const
{
  DataVector<JEMHits>::const_iterator pos  = jemHitsVec->begin();
  DataVector<JEMHits>::const_iterator pose = jemHitsVec->end();
  for (; pos != pose; ++pos) {
    const JEMHits* hits = *pos;
    int crate = hits->crate();
    int jem   = hits->module();
    int peak  = hits->peak();
    HitsVector  hitsVec(hits->JetHitsVec());
    ErrorVector err(hitsVec.size());
    cmmHitsMod->push_back(new CMMJetHits(crate, jem, hitsVec, err, peak));
  }
}

/** form partial CMM-Jet hits (crate) from module CMM-Jet hits */

void L1JEPHitsTools::formCMMJetHitsCrate(
                                   const DataVector<CMMJetHits>* cmmHitsMod,
                                   DataVector<CMMJetHits>* cmmHitsCrate) const
{
  int peakm = 0;
  HitsVector crate0Main(1);
  HitsVector crate0Fwd(1);
  HitsVector crate1Main(1);
  HitsVector crate1Fwd(1);
  DataVector<CMMJetHits>::const_iterator pos  = cmmHitsMod->begin();
  DataVector<CMMJetHits>::const_iterator pose = cmmHitsMod->end();
  for (; pos != pose; ++pos) {
    const CMMJetHits* hits = *pos;
    int dataId = hits->dataID();
    if (dataId > 15) continue;
    int crate  = hits->crate();
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hitsVec(hits->HitsVec());
    if (dataId == 0 || dataId == 7 || dataId == 8 || dataId == 15) {
      HitsVector hitsMain(main(hitsVec));
      HitsVector hitsFwd(forward(dataId, hitsVec));
      if (crate == 0) {
        addCMMJetHits(crate0Main, hitsMain, MAIN_HITS);
        addCMMJetHits(crate0Fwd,  hitsFwd,  FORWARD_HITS);
      } else {
        addCMMJetHits(crate1Main, hitsMain, MAIN_HITS);
        addCMMJetHits(crate1Fwd,  hitsFwd,  FORWARD_HITS);
      }
    } else {
      if (crate == 0) addCMMJetHits(crate0Main, hitsVec, MAIN_HITS);
      else            addCMMJetHits(crate1Main, hitsVec, MAIN_HITS);
    }
  }
  // Save non-zero crate totals
  saveCMMJetHits(cmmHitsCrate, crate0Main, 0, CMMJetHits::LOCAL_MAIN,    peakm);
  saveCMMJetHits(cmmHitsCrate, crate0Fwd,  0, CMMJetHits::LOCAL_FORWARD, peakm);
  saveCMMJetHits(cmmHitsCrate, crate1Main, 1, CMMJetHits::LOCAL_MAIN,    peakm);
  saveCMMJetHits(cmmHitsCrate, crate1Fwd,  1, CMMJetHits::LOCAL_FORWARD, peakm);
  saveCMMJetHits(cmmHitsCrate, crate0Main, 1, CMMJetHits::REMOTE_MAIN,   peakm);
  saveCMMJetHits(cmmHitsCrate, crate0Fwd,  1, CMMJetHits::REMOTE_FORWARD,
                                                                         peakm);
}

/** form partial CMM-Jet hits (system) from crate CMM-Jet hits */

void L1JEPHitsTools::formCMMJetHitsSystem(
                                 const DataVector<CMMJetHits>* cmmHitsCrate,
				 DataVector<CMMJetHits>* cmmHitsSys) const
{
  int peakm = 0;
  HitsVector systemMain(1);
  HitsVector systemFwd(1);
  DataVector<CMMJetHits>::const_iterator pos  = cmmHitsCrate->begin();
  DataVector<CMMJetHits>::const_iterator pose = cmmHitsCrate->end();
  for (; pos != pose; ++pos) {
    const CMMJetHits* hits = *pos;
    if (hits->crate() != 1) continue;
    int dataId = hits->dataID();
    if (dataId != CMMJetHits::LOCAL_MAIN    &&
        dataId != CMMJetHits::LOCAL_FORWARD &&
        dataId != CMMJetHits::REMOTE_MAIN   &&
	dataId != CMMJetHits::REMOTE_FORWARD) continue;
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hitsVec(hits->HitsVec());
    if (dataId == CMMJetHits::LOCAL_MAIN || dataId == CMMJetHits::REMOTE_MAIN) {
           addCMMJetHits(systemMain, hitsVec, MAIN_HITS);
    } else addCMMJetHits(systemFwd,  hitsVec, FORWARD_HITS);
  }
  // Save non-zero system totals
  saveCMMJetHits(cmmHitsSys, systemMain, 1, CMMJetHits::TOTAL_MAIN,    peakm);
  saveCMMJetHits(cmmHitsSys, systemFwd,  1, CMMJetHits::TOTAL_FORWARD, peakm);
}

/** form partial CMM-Jet hits (et map) from system CMM-Jet hits */

void L1JEPHitsTools::formCMMJetHitsEtMap(
                                   const DataVector<CMMJetHits>* cmmHitsSys,
				   DataVector<CMMJetHits>* cmmHitsMap) const
{
  // Get system totals
  int crate = 1;
  int peakm = 0;
  HitsVector systemMain;
  HitsVector systemFwd;
  DataVector<CMMJetHits>::const_iterator pos  = cmmHitsSys->begin();
  DataVector<CMMJetHits>::const_iterator pose = cmmHitsSys->end();
  for (; pos != pose; ++pos) {
    const CMMJetHits* hits = *pos;
    int dataId = hits->dataID();
    if (dataId != CMMJetHits::TOTAL_MAIN &&
        dataId != CMMJetHits::TOTAL_FORWARD) continue;
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    crate = hits->crate();
    if (dataId == CMMJetHits::TOTAL_MAIN)    systemMain = hits->HitsVec();
    if (dataId == CMMJetHits::TOTAL_FORWARD) systemFwd  = hits->HitsVec();
  }
  if (systemMain.empty() && systemFwd.empty()) return;
  if (systemMain.empty()) systemMain.resize(systemFwd.size());
  if (systemFwd.empty())  systemFwd.resize(systemMain.size());

  // Get current jet weights
  unsigned int nthresh = TrigT1CaloDefs::numOfJetThresholds +
                         TrigT1CaloDefs::numOfFwdJetThresholds;
  std::vector<int> factor;
  // xml location
  factor = m_configSvc->thresholdConfig()->caloInfo().jetWeights();
  if (factor.empty()) {
    // COOL location?
    factor = m_configSvc->ctpConfig()->menu().caloInfo().jetWeights();
    if (factor.empty()) {
      // Try unpacking directly
      std::vector<TrigConf::TriggerThreshold*> thresholds =
                        m_configSvc->ctpConfig()->menu().thresholdVector();
      std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
      for (it = thresholds.begin(); it != thresholds.end(); ++it) {
        if ((*it)->name() == "JetWeights") {
	  std::string cableName = (*it)->cableName();
	  std::replace(cableName.begin(), cableName.end(), ',', ' ');
	  std::istringstream weightsin(cableName+" 0 0 0 0 0 0 0 0 0 0 0 0");
	  for (unsigned int w = 0; w < nthresh; ++w) {
	    int weight;
	    weightsin >> weight;
	    factor.push_back(weight);
          }
        }
      }
      if (m_debug) {
        if (factor.empty()) {
          ATH_MSG_DEBUG( "No Jet Weights found" );
        } else {
          ATH_MSG_DEBUG( "Jet Weights from thresholds" );
        }
      }
    } else if (m_debug) {
      ATH_MSG_DEBUG( "Jet Weights from ctpConfig()->menu()" );
    }
  } else if (m_debug) {
    ATH_MSG_DEBUG( "Jet Weights from thresholdConfig()" );
  }
  factor.resize(nthresh);
  if (m_debug) {
    ATH_MSG_DEBUG( "Jet Weights from configSvc: ");
    for (std::vector<int>::const_iterator it  = factor.begin();
                                          it != factor.end(); ++it) {
      ATH_MSG_DEBUG( " " << (*it));
    }
    ATH_MSG_DEBUG(" ");
  }

  // Get current threshold values
  TrigConf::L1DataDef def;
  unsigned int maxThresh = 0;
  std::vector<unsigned int>
                   thresholdValues(TrigT1CaloDefs::numOfJetEtSumThresholds);
  std::vector<int> thresholdSet(TrigT1CaloDefs::numOfJetEtSumThresholds);
  std::vector<TrigConf::TriggerThreshold*> thresholds =
                        m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jeType() ) {
      // ET trigger thresholds can only have one global value
      TrigConf::TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0);
      unsigned int threshNum = (*it)->thresholdNumber();
      if (threshNum < TrigT1CaloDefs::numOfJetEtSumThresholds) {
	unsigned int threshVal = static_cast<unsigned>((*tv).ptcut());
	if (threshVal > maxThresh) maxThresh = threshVal;
        thresholdValues[threshNum] = threshVal;
	thresholdSet[threshNum] = 1;
      }
    }
  }
  if (m_debug) {
    ATH_MSG_DEBUG( "Jet Et Thresholds from configSvc: ");
    for (std::vector<unsigned int>::const_iterator it = thresholdValues.begin();
                                    it != thresholdValues.end(); ++it) {
      ATH_MSG_DEBUG( " " << (*it));
    }
    ATH_MSG_DEBUG(" ");
  }

  // Find number of bits to drop in Jet Et summation
  int dropBits = 0;
  maxThresh &= 0xfff;
  maxThresh >>= 8;
  while (maxThresh) {
    ++dropBits;
    maxThresh >>= 1;
  }

  // Loop over slices
  HitsVector jetEtMap;
  HitsVector::const_iterator itm = systemMain.begin();
  HitsVector::const_iterator ite = systemMain.end();
  HitsVector::const_iterator itf = systemFwd.begin();
  for (; itm != ite; ++itm, ++itf) {

    unsigned int main = *itm;
    unsigned int fwd  = *itf;
    unsigned int jetEtThreshMap = 0;
    if (main || fwd) {

      // Extract threshold multiplicities
      std::vector<unsigned int> jetMult;
      for (unsigned int thr = 0; thr < TrigT1CaloDefs::numOfJetThresholds;
                                                                     ++thr) {
        jetMult.push_back((main >> (3 * thr)) & 0x7);
      }
      for (unsigned int thr = 0; thr < TrigT1CaloDefs::numOfFwdJetThresholds;
                                                                     ++thr) {
        unsigned int left  = (fwd >> (2 * thr)) & 0x3;
        unsigned int right =
            (fwd >> (2 * (thr + TrigT1CaloDefs::numOfFwdJetThresholds))) & 0x3;
        jetMult.push_back(left + right);
      }

      // Estimate jet Et
      unsigned int jetEt = 0;
      for (unsigned int j = 0; j < nthresh; ++j) {
        jetEt += jetMult[j] * factor[j];
	// Lookup tables (2) are 8 bit - drop bits if necessary
	if ((j%4 == 3) || (j == nthresh-1)) jetEt = ((jetEt>>dropBits)<<dropBits);
        if (m_debug) {
          if (j < TrigT1CaloDefs::numOfJetThresholds) {
                 ATH_MSG_DEBUG( "Jet Threshold ");
          } else ATH_MSG_DEBUG( "Forward Jet Threshold ");
          ATH_MSG_DEBUG( j << " has multiplicity " << jetMult[j]
                                   << " giving jetEt = "   << jetEt ); 
        }
      }

      // Set the bitmap
      for (unsigned int thr = 0; thr < TrigT1CaloDefs::numOfJetEtSumThresholds;
                                                                       ++thr) {
        if (thresholdSet[thr] && jetEt > thresholdValues[thr]) {
          jetEtThreshMap |= (1 << thr);
	  ATH_MSG_DEBUG( "Passed threshold " << thr + 1
	                  << " (" << thresholdValues[thr] << " GeV)" );
        }
      }
    }
    jetEtMap.push_back(jetEtThreshMap);
  }

  // Save non-zero et map
  saveCMMJetHits(cmmHitsMap, jetEtMap, crate, CMMJetHits::ET_MAP, peakm);
}

/** Add hits from second vector to first */

void L1JEPHitsTools::addCMMJetHits(HitsVector& vec1,
                             const HitsVector& vec2, HitsType type) const
{
  int size1 = vec1.size();
  int size2 = vec2.size();
  if (size1 < size2) vec1.resize(size2);
  HitsVector::iterator pos1  = vec1.begin();
  HitsVector::iterator pose1 = vec1.end();
  HitsVector::const_iterator pos2  = vec2.begin();
  HitsVector::const_iterator pose2 = vec2.end();
  for (; pos1 != pose1 && pos2 != pose2; ++pos1, ++pos2) {
    if (type == MAIN_HITS) *pos1 = addHits(*pos1, *pos2, 24, 24, 8);
    else                   *pos1 = addHits(*pos1, *pos2, 16, 16, 8);
  }
}

/** Increment JEM/CMM hit word */

unsigned int L1JEPHitsTools::addHits(unsigned int hitMult, unsigned int hitVec,
			     int multBits, int vecBits, int nthresh) const
{
  ATH_MSG_DEBUG("addHits: Original hitMult = " << std::hex << hitMult
                   << ". Add hitWord = " << hitVec << std::dec);
  
  int nbitsOut = multBits/nthresh;
  int nbitsIn  = vecBits/nthresh;
  
  ATH_MSG_DEBUG(" Bits per threshold = " << nbitsOut);
  
  int max = (1<<nbitsOut) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = (1<<nbitsIn) - 1;
  unsigned int shift = 0;
  
  unsigned int hits = 0;
    
  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult&multMask) + (hitVec&hitMask);
    mult = ( (mult<=max) ? mult : max);
    hits += (mult<<shift);
    
    hitMult >>= nbitsOut;
    hitVec  >>= nbitsIn;
    shift += nbitsOut;
  }
  
  ATH_MSG_DEBUG("addHits returning hitMult = "
                  << std::hex << hits << std::dec );
  
  return hits;
}

/** Extract Forward hit counts from Forward+Main and split left/right */

L1JEPHitsTools::HitsVector L1JEPHitsTools::forward(int jem,
                                                   const HitsVector& vec) const
{
  HitsVector fwdVec;
  HitsVector::const_iterator pos  = vec.begin();
  HitsVector::const_iterator pose = vec.end();
  for (; pos != pose; ++pos) {
    unsigned int newHits = *pos >> 16;
    if (jem == 7 || jem == 15) newHits <<= 8;
    fwdVec.push_back(newHits);
  }
  return fwdVec;
}

/** Extract Main hit counts from Forward+Main and expand to 3 bits */

L1JEPHitsTools::HitsVector L1JEPHitsTools::main(const HitsVector& vec) const
{
  HitsVector mainVec;
  HitsVector::const_iterator pos  = vec.begin();
  HitsVector::const_iterator pose = vec.end();
  for (; pos != pose; ++pos) {
    unsigned int oldHits = *pos;
    unsigned int newHits = 0;
    for (int thr = 0; thr < 8; ++thr) {
      newHits |= ((oldHits >> (thr * 2)) & 0x3) << (thr * 3);
    }
    mainVec.push_back(newHits);
  }
  return mainVec;
}

/** Merge CMM-Jet hits vectors */

void L1JEPHitsTools::mergeCMMJetHits(DataVector<CMMJetHits>* cmmHitsVec1,
                                     DataVector<CMMJetHits>* cmmHitsVec2) const
{
  int size = cmmHitsVec2->size();
  for (int index = 0; index < size; ++index) {
    CMMJetHits* hitsIn  = 0;
    CMMJetHits* hitsOut = 0;
    cmmHitsVec2->swapElement(index, hitsIn, hitsOut);
    cmmHitsVec1->push_back(hitsOut);
  }
  cmmHitsVec2->clear();
}

/** Save non-zero CMM-Jet hits */

void L1JEPHitsTools::saveCMMJetHits(DataVector<CMMJetHits>* cmmHitsVec,
                                    const HitsVector& hits,
				    int crate, int dataId, int peak) const
{
  if (std::accumulate(hits.begin(), hits.end(), 0)) {
    ErrorVector err(hits.size());
    cmmHitsVec->push_back(new CMMJetHits(crate, dataId, hits, err, peak));
  }
}

} // end of namespace
