/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1CPHitsTools.cxx,  
///////////////////////////////////////////////////////////////////

#include <map>
#include <numeric>

#include "TrigT1CaloUtils/ClusterProcessorModuleKey.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "L1CPHitsTools.h"

namespace LVL1 {

/** Constructor */

L1CPHitsTools::L1CPHitsTools(const std::string& type,
                             const std::string& name,
                             const IInterface*  parent)
  :
  AthAlgTool(type, name, parent) 
{
  declareInterface<IL1CPHitsTools>(this);
}

/** Destructor */

L1CPHitsTools::~L1CPHitsTools()
{       
}


/** Initialisation */

StatusCode L1CPHitsTools::initialize()
{
  m_debug = msgLvl(MSG::DEBUG);
  
  ATH_MSG_INFO( "Initialization completed" );
  
  return StatusCode::SUCCESS;
}

/** Finalisation */

StatusCode L1CPHitsTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** CPAlgorithm to CPMRoI conversion */

void L1CPHitsTools::formCPMRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                     DataVector<CPMRoI>*      cpmRoiVec) const
{
  DataVector<CPAlgorithm>::const_iterator pos  = cpAlgorithmVec->begin();
  DataVector<CPAlgorithm>::const_iterator pose = cpAlgorithmVec->end();
  for (; pos != pose; ++pos) {
    CPMRoI* roi = new CPMRoI((*pos)->RoIWord());
    cpmRoiVec->push_back(roi);
  }
}

/** EmTauROI to CPMRoI conversion */

void L1CPHitsTools::formCPMRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                     DataVector<CPMRoI>*   cpmRoiVec) const
{
  DataVector<EmTauROI>::const_iterator pos  = emTauRoiVec->begin();
  DataVector<EmTauROI>::const_iterator pose = emTauRoiVec->end();
  for (; pos != pose; ++pos) {
    CPMRoI* roi = new CPMRoI((*pos)->roiWord());
    cpmRoiVec->push_back(roi);
  }
}

/** form CPM hits from RoIs - single slice */

void L1CPHitsTools::formCPMHits(const DataVector<CPMRoI>*  cpmRoiVec,
                                      DataVector<CPMHits>* cpmHitsVec) const
{
  std::vector<const DataVector<CPMRoI>*> cpmRoiColls(1, cpmRoiVec);
  formCPMHits(cpmRoiColls, cpmHitsVec, 0);
}

/** form CPM hits from RoIs - multiple slices */

void L1CPHitsTools::formCPMHits(
                    const std::vector<const DataVector<CPMRoI>*>& cpmRoiColls,
		    DataVector<CPMHits>* cpmHitsVec, int peak) const
{
  ClusterProcessorModuleKey testKey;
  std::map<unsigned int, CPMHits*> cpmHitsMap;
  DataVector<CPMRoI>::const_iterator it;
  int timeslices = cpmRoiColls.size();

  for (int slice = 0; slice < timeslices; ++slice) {
    const DataVector<CPMRoI>* cpmRoiVec = cpmRoiColls[slice];
  
    for (it=cpmRoiVec->begin(); it!=cpmRoiVec->end(); ++it){
      const CPMRoI* roi = *it;
      unsigned int crate = roi->crate();
      unsigned int module = roi->cpm();
      unsigned int key = testKey.cpmKey(crate, module);
      if (m_debug) {
        CPRoIDecoder decoder;
        CoordinateRange coord = decoder.coordinate(roi->roiWord());
	if (timeslices > 1) ATH_MSG_DEBUG( "Slice " << slice << ", ");
        ATH_MSG_DEBUG( "Found RoI with (eta, phi) = ("
              << coord.eta() << ", " << coord.phi() << ") " << ", RoIWord = "
              << std::hex << roi->roiWord() << std::dec );
        ATH_MSG_DEBUG( "Crate = " << crate << ", Module = " << module
              << ", CPM key = " << key );
      }
      CPMHits* cpmHits=0;
      // find whether corresponding CPMHits already exists
      std::map<unsigned int, CPMHits*>::iterator test=cpmHitsMap.find(key);
      // if not, create it
      if ( test==cpmHitsMap.end()){
        ATH_MSG_DEBUG( "New key. CPM has crate = " 
                           << crate << ", Module = " << module );
        ATH_MSG_DEBUG( "Create new CPMHits" ); 
        cpmHits = new CPMHits(crate, module);
	if (timeslices > 1) {
	  HitsVector hitVec(timeslices);
	  cpmHits->addHits(hitVec, hitVec);
	  cpmHits->setPeak(peak);
        }
      
        ATH_MSG_DEBUG( "and insert into map" ); 
        cpmHitsMap.insert(
                    std::map<unsigned int,CPMHits*>::value_type(key,cpmHits));
        cpmHitsVec->push_back(cpmHits);
      }
      else {
        ATH_MSG_DEBUG( "Existing CPMHits" ); 
        cpmHits = test->second; // Already exists, so set pointer
      }
      // increment hit multiplicity.
      ATH_MSG_DEBUG( "Update CPM hits" ); 
      HitsVector hitvec0(cpmHits->HitsVec0());
      HitsVector hitvec1(cpmHits->HitsVec1());
      unsigned int hits0 = hitvec0[slice];
      unsigned int hits1 = hitvec1[slice];
      hits0 = addHits(hits0, roi->roiWord()&0xFF, 24, 8);
      hits1 = addHits(hits1,(roi->roiWord()&0xFF00)>>8, 24, 8);
      hitvec0[slice] = hits0;
      hitvec1[slice] = hits1;
      cpmHits->addHits(hitvec0, hitvec1);
      ATH_MSG_DEBUG( "All done for this one" ); 
    }
  }
}

/** form complete CMM-CP hits from CPM hits */

void L1CPHitsTools::formCMMCPHits(const DataVector<CPMHits>* cpmHitsVec,
                                  DataVector<CMMCPHits>* cmmHitsVec) const
{
  DataVector<CMMCPHits>* cmmHitsCrate = new DataVector<CMMCPHits>;
  DataVector<CMMCPHits>* cmmHitsSys   = new DataVector<CMMCPHits>;
  formCMMCPHitsModule(cpmHitsVec, cmmHitsVec);
  formCMMCPHitsCrate(cmmHitsVec, cmmHitsCrate);
  formCMMCPHitsSystem(cmmHitsCrate, cmmHitsSys);
  mergeCMMCPHits(cmmHitsVec, cmmHitsCrate);
  mergeCMMCPHits(cmmHitsVec, cmmHitsSys);
  delete cmmHitsCrate;
  delete cmmHitsSys;
}

/** form partial CMM-CP hits (module) from CPM hits */

void L1CPHitsTools::formCMMCPHitsModule(const DataVector<CPMHits>* cpmHitsVec,
                                      DataVector<CMMCPHits>* cmmHitsMod) const
{
  DataVector<CPMHits>::const_iterator pos  = cpmHitsVec->begin();
  DataVector<CPMHits>::const_iterator pose = cpmHitsVec->end();
  for (; pos != pose; ++pos) {
    const CPMHits* hits = *pos;
    int crate = hits->crate();
    int cpm   = hits->module();
    int peak  = hits->peak();
    HitsVector  hits0(hits->HitsVec0());
    HitsVector  hits1(hits->HitsVec1());
    ErrorVector err0(hits0.size());
    ErrorVector err1(hits1.size());
    cmmHitsMod->push_back(new CMMCPHits(crate, cpm, hits0, hits1,
                                                    err0, err1, peak));
  }
}

/** form partial CMM-CP hits (crate) from module CMM-CP hits */

void L1CPHitsTools::formCMMCPHitsCrate(const DataVector<CMMCPHits>* cmmHitsMod,
                                     DataVector<CMMCPHits>* cmmHitsCrate) const
{
  int peakm = 0;
  HitsVector crate0Hit0(1);
  HitsVector crate0Hit1(1);
  HitsVector crate1Hit0(1);
  HitsVector crate1Hit1(1);
  HitsVector crate2Hit0(1);
  HitsVector crate2Hit1(1);
  HitsVector crate3Hit0(1);
  HitsVector crate3Hit1(1);
  DataVector<CMMCPHits>::const_iterator pos  = cmmHitsMod->begin();
  DataVector<CMMCPHits>::const_iterator pose = cmmHitsMod->end();
  for (; pos != pose; ++pos) {
    const CMMCPHits* hits = *pos;
    int dataId = hits->dataID();
    if (dataId > 14) continue;
    int crate  = hits->crate();
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hits0(hits->HitsVec0());
    HitsVector hits1(hits->HitsVec1());
    if (crate == 0) {
      addCMMCPHits(crate0Hit0, hits0);
      addCMMCPHits(crate0Hit1, hits1);
    } else if (crate == 1) {
      addCMMCPHits(crate1Hit0, hits0);
      addCMMCPHits(crate1Hit1, hits1);
    } else if (crate == 2) {
      addCMMCPHits(crate2Hit0, hits0);
      addCMMCPHits(crate2Hit1, hits1);
    } else if (crate == 3) {
      addCMMCPHits(crate3Hit0, hits0);
      addCMMCPHits(crate3Hit1, hits1);
    }
  }
  // Save non-zero crate totals
  saveCMMCPHits(cmmHitsCrate, crate0Hit0, crate0Hit1, 0, CMMCPHits::LOCAL,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate1Hit0, crate1Hit1, 1, CMMCPHits::LOCAL,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate2Hit0, crate2Hit1, 2, CMMCPHits::LOCAL,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate3Hit0, crate3Hit1, 3, CMMCPHits::LOCAL,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate0Hit0, crate0Hit1, 3, CMMCPHits::REMOTE_0,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate1Hit0, crate1Hit1, 3, CMMCPHits::REMOTE_1,
                                                                        peakm);
  saveCMMCPHits(cmmHitsCrate, crate2Hit0, crate2Hit1, 3, CMMCPHits::REMOTE_2,
                                                                        peakm);
}

/** form partial CMM-CP hits (system) from crate CMM-CP hits */

void L1CPHitsTools::formCMMCPHitsSystem(
                                 const DataVector<CMMCPHits>* cmmHitsCrate,
				 DataVector<CMMCPHits>* cmmHitsSys) const
{
  int peakm = 0;
  HitsVector systemHit0(1);
  HitsVector systemHit1(1);
  DataVector<CMMCPHits>::const_iterator pos  = cmmHitsCrate->begin();
  DataVector<CMMCPHits>::const_iterator pose = cmmHitsCrate->end();
  for (; pos != pose; ++pos) {
    const CMMCPHits* hits = *pos;
    if (hits->crate() != 3) continue;
    int dataId = hits->dataID();
    if (dataId != CMMCPHits::LOCAL    &&
        dataId != CMMCPHits::REMOTE_0 &&
        dataId != CMMCPHits::REMOTE_1 &&
	dataId != CMMCPHits::REMOTE_2) continue;
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hits0(hits->HitsVec0());
    HitsVector hits1(hits->HitsVec1());
    addCMMCPHits(systemHit0, hits0);
    addCMMCPHits(systemHit1, hits1);
  }
  // Save non-zero system totals
  saveCMMCPHits(cmmHitsSys, systemHit0, systemHit1, 3, CMMCPHits::TOTAL, peakm);
}

/** Add hits from second vector to first */

void L1CPHitsTools::addCMMCPHits(HitsVector& vec1, const HitsVector& vec2) const
{
  int size1 = vec1.size();
  int size2 = vec2.size();
  if (size1 < size2) vec1.resize(size2);
  HitsVector::iterator pos1  = vec1.begin();
  HitsVector::iterator pose1 = vec1.end();
  HitsVector::const_iterator pos2  = vec2.begin();
  HitsVector::const_iterator pose2 = vec2.end();
  for (; pos1 != pose1 && pos2 != pose2; ++pos1, ++pos2) {
    *pos1 = addHits(*pos1, *pos2, 24, 24);
  }
}

/** Increment CPM/CMM hit word */

unsigned int L1CPHitsTools::addHits(unsigned int hitMult,
                                    unsigned int hitVec,
				    int multBits, int vecBits) const
{
  ATH_MSG_DEBUG("addHits: Original hitMult = " << std::hex << hitMult
                 << ". Add hitWord = " << hitVec << std::dec);
  
  // Results transmitted in 2 words, each reporting half of the CP thresholds
  int nthresh = TrigT1CaloDefs::numOfCPThresholds/2;
  
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

/** Merge CMM-CP hits vectors */

void L1CPHitsTools::mergeCMMCPHits(DataVector<CMMCPHits>* cmmHitsVec1,
                                   DataVector<CMMCPHits>* cmmHitsVec2) const
{
  int size = cmmHitsVec2->size();
  for (int index = 0; index < size; ++index) {
    CMMCPHits* hitsIn  = 0;
    CMMCPHits* hitsOut = 0;
    cmmHitsVec2->swapElement(index, hitsIn, hitsOut);
    cmmHitsVec1->push_back(hitsOut);
  }
  cmmHitsVec2->clear();
}

/** Save non-zero CMM-CP hits */

void L1CPHitsTools::saveCMMCPHits(DataVector<CMMCPHits>* cmmHitsVec,
                                  const HitsVector& hits0,
				  const HitsVector& hits1,
				  int crate, int dataId, int peak) const
{
  if (std::accumulate(hits0.begin(), hits0.end(), 0) ||
      std::accumulate(hits1.begin(), hits1.end(), 0)) {
    ErrorVector err0(hits0.size());
    ErrorVector err1(hits1.size());
    cmmHitsVec->push_back(new CMMCPHits(crate, dataId, hits0, hits1,
                                                       err0, err1, peak));
  }
}

} // end of namespace
