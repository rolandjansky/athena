/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMHitsCollectionCNV_TLP1_H
#define CPMHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CPMHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMHits and CPMHits_p1
 *
 *          Simple converter that can create CPMHits objects from
 *          CPMHits_p1 ones, and vice versa. It is used in CPMHitsCnv
 *          to do the T/P conversion before/after writing/reading the CPMHits
 *          object.
 */
class CPMHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CPMHitsCollectionCnv_p1, CPMHitsCollection_tlp1 > {

public:
  CPMHitsCollectionCnv_tlp1();
  virtual ~CPMHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CPMHitsCollection_tlp1 *storage );

protected:
   CPMHitsCollectionCnv_p1  m_CPMHitsCollectionCnv;
   CPMHitsCnv_p1            m_CPMHitsCnv;

}; // class CPMHitsCollectionCnv_tlp1

#endif // CPMHitsCollectionCNV_TLP1_H
