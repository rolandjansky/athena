/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMap_tlp3
 *
 * @brief "top level" persistent partner for TrigInDetTrackTruthMap
 *
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_TLP3_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_TLP3_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruth_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStats_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

class TrigInDetTrackTruthMap_tlp3 {

 public:
  TrigInDetTrackTruthMap_tlp3() {}

  //vectors of ALL persistent classes in TrigInDetTrackTruthMap
  std::vector<TrigInDetTrackTruthMap_p1>    m_trigInDetTrackTruthMap_p1;
  std::vector<TrigInDetTrackTruth_p1>       m_trigInDetTrackTruth_p1;
  std::vector<TrigIDHitStats_p1>            m_trigIDHitStats_p1;
  std::vector<HepMcParticleLink_p2>         m_hepMcParticleLink_p2;

};

#endif // TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_TLP3_H
