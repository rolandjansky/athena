/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXCPHitsCollectionCNV_TLP1_H
#define CMXCPHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMXCPHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXCPHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXCPHits and CMXCPHits_p1
 *
 *          Simple converter that can create CMXCPHits objects from
 *          CMXCPHits_p1 ones, and vice versa. It is used in CMXCPHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMXCPHits
 *          object.
 */
class CMXCPHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMXCPHitsCollectionCnv_p1, CMXCPHitsCollection_tlp1 > {

public:
  CMXCPHitsCollectionCnv_tlp1();
  virtual ~CMXCPHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMXCPHitsCollection_tlp1 *storage );

protected:
   CMXCPHitsCollectionCnv_p1  m_CMXCPHitsCollectionCnv;
   CMXCPHitsCnv_p1            m_CMXCPHitsCnv;

}; // class CMXCPHitsCollectionCnv_tlp1

#endif // CMXCPHitsCollectionCNV_TLP1_H
