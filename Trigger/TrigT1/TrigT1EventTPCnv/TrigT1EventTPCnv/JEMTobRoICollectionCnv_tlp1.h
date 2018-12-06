/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMTobRoICollectionCNV_TLP1_H
#define JEMTobRoICollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/JEMTobRoICollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMTobRoICollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMTobRoI and JEMTobRoI_p1
 *
 *          Simple converter that can create JEMTobRoI objects from
 *          JEMTobRoI_p1 ones, and vice versa. It is used in JEMTobRoICnv
 *          to do the T/P conversion before/after writing/reading the JEMTobRoI
 *          object.
 */
class JEMTobRoICollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< JEMTobRoICollectionCnv_p1, JEMTobRoICollection_tlp1 > {

public:
  JEMTobRoICollectionCnv_tlp1();
  virtual ~JEMTobRoICollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( JEMTobRoICollection_tlp1 *storage );

protected:
   JEMTobRoICollectionCnv_p1  m_JEMTobRoICollectionCnv;
   JEMTobRoICnv_p1            m_JEMTobRoICnv;

}; // class JEMTobRoICollectionCnv_tlp1

#endif // JEMTobRoICollectionCNV_TLP1_H
