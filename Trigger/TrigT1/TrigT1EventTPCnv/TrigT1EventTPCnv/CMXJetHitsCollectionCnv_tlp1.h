/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetHitsCollectionCNV_TLP1_H
#define CMXJetHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMXJetHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXJetHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXJetHits and CMXJetHits_p1
 *
 *          Simple converter that can create CMXJetHits objects from
 *          CMXJetHits_p1 ones, and vice versa. It is used in CMXJetHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMXJetHits
 *          object.
 */
class CMXJetHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMXJetHitsCollectionCnv_p1, CMXJetHitsCollection_tlp1 > {

public:
  CMXJetHitsCollectionCnv_tlp1();
  virtual ~CMXJetHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMXJetHitsCollection_tlp1 *storage );

protected:
   CMXJetHitsCollectionCnv_p1  m_CMXJetHitsCollectionCnv;
   CMXJetHitsCnv_p1            m_CMXJetHitsCnv;

}; // class CMXJetHitsCollectionCnv_tlp1

#endif // CMXJetHitsCollectionCNV_TLP1_H
