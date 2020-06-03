/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetTobCollectionCNV_P1_H
#define CMXJetTobCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXJetTobCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXJetTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetTobCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXJetTob and CMXJetTob_p1
 *
 *          Simple converter that can create CMXJetTob objects from
 *          CMXJetTob_p1 ones, and vice versa. It is used in CMXJetTobCnv
 *          to do the T/P conversion before/after writing/reading the CMXJetTob
 *          object.
 */
class CMXJetTobCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMXJetTobCollection, CMXJetTobCollection_p1, CMXJetTobCnv_p1 > {

public:
  CMXJetTobCollectionCnv_p1() {};

}; // class CMXJetTobCollectionCnv_p1

#endif // CMXJetTobCollectionCNV_P1_H
