/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTowerCollectionCNV_TLP1_H
#define CPMTowerCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/CPMTowerCollection_tlp1.h"
#include "TrigT1EventTPCnv/CPMTowerCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMTower and CPMTower_p1
 *
 *          Simple converter that can create CPMTower objects from
 *          CPMTower_p1 ones, and vice versa. It is used in CPMTowerCnv
 *          to do the T/P conversion before/after writing/reading the CPMTower
 *          object.
 */
class CPMTowerCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CPMTowerCollectionCnv_p1, CPMTowerCollection_tlp1 > {

public:
  CPMTowerCollectionCnv_tlp1();
  virtual ~CPMTowerCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( CPMTowerCollection_tlp1 *storage );

protected:
   CPMTowerCollectionCnv_p1  m_CPMTowerCollectionCnv;
   CPMTowerCnv_p1            m_CPMTowerCnv;

}; // class CPMTowerCollectionCnv_tlp1

#endif // CPMTowerCollectionCNV_TLP1_H
