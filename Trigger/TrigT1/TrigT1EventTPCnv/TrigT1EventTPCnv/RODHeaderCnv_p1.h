/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef RODHeaderCNV_P1_H
#define RODHeaderCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/RODHeader.h"

// Local include(s):
#include "TrigT1EventTPCnv/RODHeader_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for RODHeader and RODHeader_p1
 *
 *          Simple converter that can create RODHeader objects from
 *          RODHeader_p1 ones, and vice versa. It is used in RODHeaderCnv
 *          to do the T/P conversion before/after writing/reading the RODHeader
 *          object.
 */
class RODHeaderCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::RODHeader, RODHeader_p1 > {

public:
  RODHeaderCnv_p1() : m_RODHeaderCnv(0) {}

  virtual void persToTrans( const RODHeader_p1* persObj, LVL1::RODHeader* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::RODHeader* transObj, RODHeader_p1* persObj, MsgStream &log );

protected:
  RODHeaderCnv_p1 *m_RODHeaderCnv ;

}; // class RODHeaderCnv_p1

#endif // RODHeaderCNV_P1_H
