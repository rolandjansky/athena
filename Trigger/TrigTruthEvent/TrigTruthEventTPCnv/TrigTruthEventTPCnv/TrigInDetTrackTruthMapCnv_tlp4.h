/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMapCnv_tlp4
 *
 * @brief "top level" persistent partner for TrigInDetTrackTruthMapCnv
 *
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP4_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP4_H

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp4.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p3.h"


class TrigInDetTrackTruthMapCnv_tlp4 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackTruthMapCnv_p1, TrigInDetTrackTruthMap_tlp4>  {

 public:
  TrigInDetTrackTruthMapCnv_tlp4();
  virtual ~TrigInDetTrackTruthMapCnv_tlp4() {}

  virtual void setPStorage(TrigInDetTrackTruthMap_tlp4 *storage);


 protected:

  //tp converters for ALL converter types used in TrigInDetTrackTruthMapCnv
  TrigInDetTrackTruthMapCnv_p1    m_trigInDetTrackTruthMapCnv_p1;
  TrigInDetTrackTruthCnv_p1       m_trigInDetTrackTruthCnv_p1;
  TrigIDHitStatsCnv_p1            m_trigIDHitStatsCnv_p1;
  HepMcParticleLinkCnv_p3         m_hepMcParticleLinkCnv_p3;

};


#endif // TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP4_H
