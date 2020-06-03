/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMRoICollectionCNV_TLP1_H
#define CPMRoICollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CPMRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMRoICollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMRoI and CPMRoI_p1
 *
 *          Simple converter that can create CPMRoI objects from
 *          CPMRoI_p1 ones, and vice versa. It is used in CPMRoICnv
 *          to do the T/P conversion before/after writing/reading the CPMRoI
 *          object.
 */
class CPMRoICollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CPMRoICollectionCnv_p1, CPMRoICollection_tlp1 > {

public:
  CPMRoICollectionCnv_tlp1();
  virtual ~CPMRoICollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CPMRoICollection_tlp1 *storage );

protected:
   CPMRoICollectionCnv_p1  m_CPMRoICollectionCnv;
   CPMRoICnv_p1            m_CPMRoICnv;

}; // class CPMRoICollectionCnv_tlp1

#endif // CPMRoICollectionCNV_TLP1_H
