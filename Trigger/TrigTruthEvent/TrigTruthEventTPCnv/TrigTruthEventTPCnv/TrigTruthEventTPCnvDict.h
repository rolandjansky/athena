/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRUTHEVENTTPCNV_TRIGTRUTHEVENTTPCNVDICT_H
#define TRIGTRUTHEVENTTPCNV_TRIGTRUTHEVENTTPCNVDICT_H

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp2.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp3.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old_cnv.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruth_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStats_p1.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigEventTPCnvDict{
  
  std::vector<TrigInDetTrackTruthMap_p1>            m_dummyTrigInDetTrackTruthMap_p1;
  std::vector<TrigInDetTrackTruth_p1>               m_dummyTrigInDetTrackTruth_p1;
  std::vector<TrigIDHitStats_p1> 	            m_dummyTrigIDHitStats_p1;
  std::vector< std::pair<unsigned int, unsigned int> >   m_dummyPairUnsignInt;

};
 
#endif 
