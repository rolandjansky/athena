/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMHitsCollectionCNV_TLP1_H
#define JEMHitsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/JEMHitsCollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMHitsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMHits and JEMHits_p1
 *
 *          Simple converter that can create JEMHits objects from
 *          JEMHits_p1 ones, and vice versa. It is used in JEMHitsCnv
 *          to do the T/P conversion before/after writing/reading the JEMHits
 *          object.
 */
class JEMHitsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< JEMHitsCollectionCnv_p1, JEMHitsCollection_tlp1 > {

public:
  JEMHitsCollectionCnv_tlp1();
  virtual ~JEMHitsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( JEMHitsCollection_tlp1 *storage );

protected:
   JEMHitsCollectionCnv_p1  m_JEMHitsCollectionCnv;
   JEMHitsCnv_p1            m_JEMHitsCnv;

}; // class JEMHitsCollectionCnv_tlp1

#endif // JEMHitsCollectionCNV_TLP1_H
