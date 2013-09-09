/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassFlagsCnv_p1
 *
 * @brief transient persistent converter for TrigPassFlags
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigPassFlagsCnv_p1_H
#define TrigSteeringEventTPCnv_TrigPassFlagsCnv_p1_H

#include "TrigSteeringEvent/TrigPassFlags.h"
#include "TrigSteeringEventTPCnv/TrigPassFlags_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigPassFlagsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigPassFlags, TrigPassFlags_p1>
{
public:

  TrigPassFlagsCnv_p1() {}

  virtual void persToTrans(const TrigPassFlags_p1* persObj, 
                           TrigPassFlags* transObj, 
                           MsgStream &log);
  virtual void transToPers(const TrigPassFlags* transObj, 
                           TrigPassFlags_p1* persObj, 
                           MsgStream &log);
};


#endif
