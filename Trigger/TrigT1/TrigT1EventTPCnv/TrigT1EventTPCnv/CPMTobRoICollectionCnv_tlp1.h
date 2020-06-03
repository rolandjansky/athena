/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTobRoICollectionCNV_TLP1_H
#define CPMTobRoICollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CPMTobRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMTobRoICollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMTobRoI and CPMTobRoI_p1
 *
 *          Simple converter that can create CPMTobRoI objects from
 *          CPMTobRoI_p1 ones, and vice versa. It is used in CPMTobRoICnv
 *          to do the T/P conversion before/after writing/reading the CPMTobRoI
 *          object.
 */
class CPMTobRoICollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CPMTobRoICollectionCnv_p1, CPMTobRoICollection_tlp1 > {

public:
  CPMTobRoICollectionCnv_tlp1();
  virtual ~CPMTobRoICollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CPMTobRoICollection_tlp1 *storage );

protected:
   CPMTobRoICollectionCnv_p1  m_CPMTobRoICollectionCnv;
   CPMTobRoICnv_p1            m_CPMTobRoICnv;

}; // class CPMTobRoICollectionCnv_tlp1

#endif // CPMTobRoICollectionCNV_TLP1_H
