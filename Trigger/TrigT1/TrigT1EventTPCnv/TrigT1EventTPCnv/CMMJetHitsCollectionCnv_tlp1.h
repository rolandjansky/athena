/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMJetHitsCollectionCNV_TLP1_H
#define CMMJetHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMMJetHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMJetHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMJetHits and CMMJetHits_p1
 *
 *          Simple converter that can create CMMJetHits objects from
 *          CMMJetHits_p1 ones, and vice versa. It is used in CMMJetHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMMJetHits
 *          object.
 */
class CMMJetHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMMJetHitsCollectionCnv_p1, CMMJetHitsCollection_tlp1 > {

public:
  CMMJetHitsCollectionCnv_tlp1();
  virtual ~CMMJetHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMMJetHitsCollection_tlp1 *storage );

protected:
   CMMJetHitsCollectionCnv_p1  m_CMMJetHitsCollectionCnv;
   CMMJetHitsCnv_p1            m_CMMJetHitsCnv;

}; // class CMMJetHitsCollectionCnv_tlp1

#endif // CMMJetHitsCollectionCNV_TLP1_H
