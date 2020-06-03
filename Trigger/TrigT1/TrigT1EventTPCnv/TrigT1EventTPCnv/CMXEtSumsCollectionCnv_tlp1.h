/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXEtSumsCollectionCNV_TLP1_H
#define CMXEtSumsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMXEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXEtSumsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXEtSums and CMXEtSums_p1
 *
 *          Simple converter that can create CMXEtSums objects from
 *          CMXEtSums_p1 ones, and vice versa. It is used in CMXEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the CMXEtSums
 *          object.
 */
class CMXEtSumsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMXEtSumsCollectionCnv_p1, CMXEtSumsCollection_tlp1 > {

public:
  CMXEtSumsCollectionCnv_tlp1();
  virtual ~CMXEtSumsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMXEtSumsCollection_tlp1 *storage );

protected:
   CMXEtSumsCollectionCnv_p1  m_CMXEtSumsCollectionCnv;
   CMXEtSumsCnv_p1            m_CMXEtSumsCnv;

}; // class CMXEtSumsCollectionCnv_tlp1

#endif // CMXEtSumsCollectionCNV_TLP1_H
