/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXCPTobCollectionCNV_TLP1_H
#define CMXCPTobCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMXCPTobCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXCPTobCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXCPTob and CMXCPTob_p1
 *
 *          Simple converter that can create CMXCPTob objects from
 *          CMXCPTob_p1 ones, and vice versa. It is used in CMXCPTobCnv
 *          to do the T/P conversion before/after writing/reading the CMXCPTob
 *          object.
 */
class CMXCPTobCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMXCPTobCollectionCnv_p1, CMXCPTobCollection_tlp1 > {

public:
  CMXCPTobCollectionCnv_tlp1();
  virtual ~CMXCPTobCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMXCPTobCollection_tlp1 *storage );

protected:
   CMXCPTobCollectionCnv_p1  m_CMXCPTobCollectionCnv;
   CMXCPTobCnv_p1            m_CMXCPTobCnv;

}; // class CMXCPTobCollectionCnv_tlp1

#endif // CMXCPTobCollectionCNV_TLP1_H
