/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMapCnv_tlp3
 *
 * @brief "top level" persistent partner for TrigInDetTrackTruthMapCnv
 *
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP3_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP3_H

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp3.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"


class TrigInDetTrackTruthMapCnv_tlp3 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackTruthMapCnv_p1, TrigInDetTrackTruthMap_tlp3>  {

 public:
  TrigInDetTrackTruthMapCnv_tlp3();
  virtual ~TrigInDetTrackTruthMapCnv_tlp3() {}

  virtual void setPStorage(TrigInDetTrackTruthMap_tlp3 *storage);


 protected:

  //tp converters for ALL converter types used in TrigInDetTrackTruthMapCnv
  TrigInDetTrackTruthMapCnv_p1    m_trigInDetTrackTruthMapCnv_p1;
  TrigInDetTrackTruthCnv_p1       m_trigInDetTrackTruthCnv_p1;
  TrigIDHitStatsCnv_p1            m_trigIDHitStatsCnv_p1;
  HepMcParticleLinkCnv_p2         m_hepMcParticleLinkCnv_p2;

};


#endif // TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP3_H
