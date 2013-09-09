/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassBitsCnv_p1
 *
 * @brief transient persistent converter for TrigPassBits
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigPassBitsCnv_p1_H
#define TrigSteeringEventTPCnv_TrigPassBitsCnv_p1_H

#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEventTPCnv/TrigPassBits_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigPassBitsCnv_p1 : public T_AthenaPoolTPCnvBase<TrigPassBits, TrigPassBits_p1>
{
public:

  TrigPassBitsCnv_p1() {}

  virtual void persToTrans(const TrigPassBits_p1* persObj, 
			   TrigPassBits* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigPassBits* transObj, 
			   TrigPassBits_p1* persObj, 
			   MsgStream &log);
};

#endif
