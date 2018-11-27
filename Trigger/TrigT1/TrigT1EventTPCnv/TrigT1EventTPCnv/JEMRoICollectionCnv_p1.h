/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMRoICollectionCNV_P1_H
#define JEMRoICollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMRoICollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMRoICnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMRoI and JEMRoI_p1
 *
 *          Simple converter that can create JEMRoI objects from
 *          JEMRoI_p1 ones, and vice versa. It is used in JEMRoICnv
 *          to do the T/P conversion before/after writing/reading the JEMRoI
 *          object.
 */
class JEMRoICollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< JEMRoICollection, JEMRoICollection_p1, JEMRoICnv_p1 > {

public:
  JEMRoICollectionCnv_p1() {};

}; // class JEMRoICollectionCnv_p1

#endif // JEMRoICollectionCNV_P1_H
