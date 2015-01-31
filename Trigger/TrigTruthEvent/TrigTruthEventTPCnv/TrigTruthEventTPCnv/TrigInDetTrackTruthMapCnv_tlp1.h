/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @Class  : TrigInDetTrackTruthMapCnv_tlp1
 *
 * @brief "top level" persistent partner for TrigInDetTrackTruthMapCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigInDetTrackTruthMapCnv_tlp1.h,v 1.3 2009-04-06 11:14:46 salvator Exp $
 **********************************************************************************/
#ifndef TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP1_H
#define TRIGTRUTHEVENTTPCNV_TRIGINDETTRACK_TRUTH_MAP_CNV_TLP1_H

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_tlp1.h"

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h"

class TrigInDetTrackTruthMapCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigInDetTrackTruthMapCnv_p1, TrigInDetTrackTruthMap_tlp1>  {

 public:
  TrigInDetTrackTruthMapCnv_tlp1();
  virtual ~TrigInDetTrackTruthMapCnv_tlp1() {}

  virtual void setPStorage(TrigInDetTrackTruthMap_tlp1 *storage);

  
 protected:

  //tp converters for ALL converter types used in TrigInDetTrackTruthMapCnv
  TrigInDetTrackTruthMapCnv_p1    m_trigInDetTrackTruthMapCnv_p1;
  TrigInDetTrackTruthCnv_p1       m_trigInDetTrackTruthCnv_p1;
  TrigIDHitStatsCnv_p1            m_trigIDHitStatsCnv_p1;
  HepMcParticleLinkCnv_p1         m_hepMcParticleLinkCnv_p1;

  TrigInDetTrackCollectionCnv_p1  m_trigInDetTrackCollectionCnv_p1;
  TrigInDetTrackCnv_p1            m_trigInDetTrackCnv_p1;
  TrigInDetTrackFitParCnv_p1      m_trigInDetTrackFitParCnv_p1;
 

};


#endif
