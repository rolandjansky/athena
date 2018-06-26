/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMTobRoICollectionCNV_P1_H
#define JEMTobRoICollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMTobRoICollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMTobRoICnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMTobRoI and JEMTobRoI_p1
 *
 *          Simple converter that can create JEMTobRoI objects from
 *          JEMTobRoI_p1 ones, and vice versa. It is used in JEMTobRoICnv
 *          to do the T/P conversion before/after writing/reading the JEMTobRoI
 *          object.
 */
class JEMTobRoICollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< JEMTobRoICollection, JEMTobRoICollection_p1, JEMTobRoICnv_p1 > {

public:
  JEMTobRoICollectionCnv_p1() {};

}; // class JEMTobRoICollectionCnv_p1

#endif // JEMTobRoICollectionCNV_P1_H
