/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGEREVENTTPCNV_LVL1_ROICNV_P1_H
#define ANALYSISTRIGGEREVENTTPCNV_LVL1_ROICNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "AnalysisTriggerEventTPCnv/LVL1_ROI_p1.h"

/**
 *   @short T/P converter for LVL1_ROI and LVL1_ROI_p1
 *
 *          Simple converter that can create LVL1_ROI objects from
 *          LVL1_ROI_p1 ones, and vice versa. It is used in LVL1_ROICnv
 *          to do the T/P conversion before/after writing/reading the LVL1_ROI
 *          object.
 */
class LVL1_ROICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1_ROI, LVL1_ROI_p1 > {

public:
  LVL1_ROICnv_p1();

  virtual void persToTrans( const LVL1_ROI_p1* persObj, LVL1_ROI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1_ROI* transObj, LVL1_ROI_p1* persObj, MsgStream &log );

}; // class LVL1_ROICnv_p1

template<>
class T_TPCnv<LVL1_ROI, LVL1_ROI_p1>
  : public LVL1_ROICnv_p1
{
public:
};

#endif // ANALYSISTRIGGEREVENTTPCNV_LVL1_ROICNV_P1_H
