/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JetElementCollectionCNV_TLP1_H
#define JetElementCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/JetElementCollection_tlp1.h"
#include "TrigT1EventTPCnv/JetElementCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JetElement and JetElement_p1
 *
 *          Simple converter that can create JetElement objects from
 *          JetElement_p1 ones, and vice versa. It is used in JetElementCnv
 *          to do the T/P conversion before/after writing/reading the JetElement
 *          object.
 */
class JetElementCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< JetElementCollectionCnv_p1, JetElementCollection_tlp1 > {

public:
  JetElementCollectionCnv_tlp1();
  virtual ~JetElementCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( JetElementCollection_tlp1 *storage );

protected:
   JetElementCollectionCnv_p1  m_JetElementCollectionCnv;
   JetElementCnv_p1            m_JetElementCnv;

}; // class JetElementCollectionCnv_tlp1

#endif // JetElementCollectionCNV_TLP1_H
