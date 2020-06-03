/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMRoICollectionCNV_TLP1_H
#define JEMRoICollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/JEMRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMRoICollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMRoI and JEMRoI_p1
 *
 *          Simple converter that can create JEMRoI objects from
 *          JEMRoI_p1 ones, and vice versa. It is used in JEMRoICnv
 *          to do the T/P conversion before/after writing/reading the JEMRoI
 *          object.
 */
class JEMRoICollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< JEMRoICollectionCnv_p1, JEMRoICollection_tlp1 > {

public:
  JEMRoICollectionCnv_tlp1();
  virtual ~JEMRoICollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( JEMRoICollection_tlp1 *storage );

protected:
   JEMRoICollectionCnv_p1  m_JEMRoICollectionCnv;
   JEMRoICnv_p1            m_JEMRoICnv;

}; // class JEMRoICollectionCnv_tlp1

#endif // JEMRoICollectionCNV_TLP1_H
