/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMEtSumsCollectionCNV_P1_H
#define JEMEtSumsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMEtSumsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMEtSumsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMEtSums and JEMEtSums_p1
 *
 *          Simple converter that can create JEMEtSums objects from
 *          JEMEtSums_p1 ones, and vice versa. It is used in JEMEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the JEMEtSums
 *          object.
 */
class JEMEtSumsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< JEMEtSumsCollection, JEMEtSumsCollection_p1, JEMEtSumsCnv_p1 > {

public:
  JEMEtSumsCollectionCnv_p1() {};

}; // class JEMEtSumsCollectionCnv_p1

#endif // JEMEtSumsCollectionCNV_P1_H
