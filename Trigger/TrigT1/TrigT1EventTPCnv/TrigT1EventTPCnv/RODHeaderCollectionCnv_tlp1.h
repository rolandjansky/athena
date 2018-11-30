/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RODHeaderCollectionCNV_TLP1_H
#define RODHeaderCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/RODHeaderCollection_tlp1.h"
#include "TrigT1EventTPCnv/RODHeaderCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for RODHeader and RODHeader_p1
 *
 *          Simple converter that can create RODHeader objects from
 *          RODHeader_p1 ones, and vice versa. It is used in RODHeaderCnv
 *          to do the T/P conversion before/after writing/reading the RODHeader
 *          object.
 */
class RODHeaderCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< RODHeaderCollectionCnv_p1, RODHeaderCollection_tlp1 > {

public:
  RODHeaderCollectionCnv_tlp1();
  virtual ~RODHeaderCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( RODHeaderCollection_tlp1 *storage );

protected:
   RODHeaderCollectionCnv_p1  m_RODHeaderCollectionCnv;
   RODHeaderCnv_p1            m_RODHeaderCnv;

}; // class RODHeaderCollectionCnv_tlp1

#endif // RODHeaderCollectionCNV_TLP1_H
