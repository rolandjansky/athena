/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMCPHitsCollectionCNV_TLP1_H
#define CMMCPHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMMCPHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMCPHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMCPHits and CMMCPHits_p1
 *
 *          Simple converter that can create CMMCPHits objects from
 *          CMMCPHits_p1 ones, and vice versa. It is used in CMMCPHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMMCPHits
 *          object.
 */
class CMMCPHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMMCPHitsCollectionCnv_p1, CMMCPHitsCollection_tlp1 > {

public:
  CMMCPHitsCollectionCnv_tlp1();
  virtual ~CMMCPHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMMCPHitsCollection_tlp1 *storage );

protected:
   CMMCPHitsCollectionCnv_p1  m_CMMCPHitsCollectionCnv;
   CMMCPHitsCnv_p1            m_CMMCPHitsCnv;

}; // class CMMCPHitsCollectionCnv_tlp1

#endif // CMMCPHitsCollectionCNV_TLP1_H
