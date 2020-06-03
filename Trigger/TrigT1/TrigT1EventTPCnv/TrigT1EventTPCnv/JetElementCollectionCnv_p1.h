/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetElementCollectionCNV_P1_H
#define JetElementCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JetElementCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/JetElementCollection_p1.h"
#include "TrigT1EventTPCnv/JetElementCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JetElement and JetElement_p1
 *
 *          Simple converter that can create JetElement objects from
 *          JetElement_p1 ones, and vice versa. It is used in JetElementCnv
 *          to do the T/P conversion before/after writing/reading the JetElement
 *          object.
 */
class JetElementCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< JetElementCollection, JetElementCollection_p1, JetElementCnv_p1 > {

public:
  JetElementCollectionCnv_p1() {};

}; // class JetElementCollectionCnv_p1

#endif // JetElementCollectionCNV_P1_H
