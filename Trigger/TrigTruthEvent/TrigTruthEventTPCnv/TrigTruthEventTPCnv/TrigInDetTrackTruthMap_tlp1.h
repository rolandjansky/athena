/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMap_tlp1
 *
 * @brief "top level" persistent partner for TrigInDetTrackTruthMap
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthMap_tlp1.h,v 1.2 2009-04-06 11:14:46 salvator Exp $
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_TLP1_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruth_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStats_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitPar_p1.h"


class TrigInDetTrackTruthMap_tlp1 {

 public:
  TrigInDetTrackTruthMap_tlp1() {}

  //vectors of ALL persistent classes in TrigInDetTrackTruthMap  
  std::vector<TrigInDetTrackTruthMap_p1>    m_trigInDetTrackTruthMap_p1;
  std::vector<TrigInDetTrackTruth_p1>       m_trigInDetTrackTruth_p1;
  std::vector<TrigIDHitStats_p1>            m_trigIDHitStats_p1;
  std::vector<HepMcParticleLink_p1>         m_hepMcParticleLink_p1;

  std::vector<TrigInDetTrackCollection_p1>  m_trigInDetTrackCollection_p1;
  std::vector<TrigInDetTrack_p1>            m_trigInDetTrack_p1;
  std::vector<TrigInDetTrackFitPar_p1>      m_trigInDetTrackFitPar_p1;
 



};

#endif

