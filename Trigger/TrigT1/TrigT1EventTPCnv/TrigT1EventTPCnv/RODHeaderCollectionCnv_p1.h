/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RODHeaderCollectionCNV_P1_H
#define RODHeaderCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/RODHeaderCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/RODHeaderCollection_p1.h"
#include "TrigT1EventTPCnv/RODHeaderCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for RODHeader and RODHeader_p1
 *
 *          Simple converter that can create RODHeader objects from
 *          RODHeader_p1 ones, and vice versa. It is used in RODHeaderCnv
 *          to do the T/P conversion before/after writing/reading the RODHeader
 *          object.
 */
class RODHeaderCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< RODHeaderCollection, RODHeaderCollection_p1, RODHeaderCnv_p1 > {

public:
  RODHeaderCollectionCnv_p1() {};

}; // class RODHeaderCollectionCnv_p1

#endif // RODHeaderCollectionCNV_P1_H
