/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMEtSumsCollectionCNV_TLP1_H
#define CMMEtSumsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMMEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMMEtSumsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMEtSums and CMMEtSums_p1
 *
 *          Simple converter that can create CMMEtSums objects from
 *          CMMEtSums_p1 ones, and vice versa. It is used in CMMEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the CMMEtSums
 *          object.
 */
class CMMEtSumsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMMEtSumsCollectionCnv_p1, CMMEtSumsCollection_tlp1 > {

public:
  CMMEtSumsCollectionCnv_tlp1();
  virtual ~CMMEtSumsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMMEtSumsCollection_tlp1 *storage );

protected:
   CMMEtSumsCollectionCnv_p1  m_CMMEtSumsCollectionCnv;
   CMMEtSumsCnv_p1            m_CMMEtSumsCnv;

}; // class CMMEtSumsCollectionCnv_tlp1

#endif // CMMEtSumsCollectionCNV_TLP1_H
