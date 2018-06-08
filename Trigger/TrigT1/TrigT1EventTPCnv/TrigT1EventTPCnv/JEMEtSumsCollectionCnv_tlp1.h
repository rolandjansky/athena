/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMEtSumsCollectionCNV_TLP1_H
#define JEMEtSumsCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/JEMEtSumsCollection_tlp1.h"
#include "TrigT1EventTPCnv/JEMEtSumsCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMEtSums and JEMEtSums_p1
 *
 *          Simple converter that can create JEMEtSums objects from
 *          JEMEtSums_p1 ones, and vice versa. It is used in JEMEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the JEMEtSums
 *          object.
 */
class JEMEtSumsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< JEMEtSumsCollectionCnv_p1, JEMEtSumsCollection_tlp1 > {

public:
  JEMEtSumsCollectionCnv_tlp1();
  virtual ~JEMEtSumsCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( JEMEtSumsCollection_tlp1 *storage );

protected:
   JEMEtSumsCollectionCnv_p1  m_JEMEtSumsCollectionCnv;
   JEMEtSumsCnv_p1            m_JEMEtSumsCnv;

}; // class JEMEtSumsCollectionCnv_tlp1

#endif // JEMEtSumsCollectionCNV_TLP1_H
