/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetTobCollectionCNV_TLP1_H
#define CMXJetTobCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CMXJetTobCollection_tlp1.h"
#include "TrigT1EventTPCnv/CMXJetTobCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXJetTob and CMXJetTob_p1
 *
 *          Simple converter that can create CMXJetTob objects from
 *          CMXJetTob_p1 ones, and vice versa. It is used in CMXJetTobCnv
 *          to do the T/P conversion before/after writing/reading the CMXJetTob
 *          object.
 */
class CMXJetTobCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CMXJetTobCollectionCnv_p1, CMXJetTobCollection_tlp1 > {

public:
  CMXJetTobCollectionCnv_tlp1();
  virtual ~CMXJetTobCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CMXJetTobCollection_tlp1 *storage );

protected:
   CMXJetTobCollectionCnv_p1  m_CMXJetTobCollectionCnv;
   CMXJetTobCnv_p1            m_CMXJetTobCnv;

}; // class CMXJetTobCollectionCnv_tlp1

#endif // CMXJetTobCollectionCNV_TLP1_H
