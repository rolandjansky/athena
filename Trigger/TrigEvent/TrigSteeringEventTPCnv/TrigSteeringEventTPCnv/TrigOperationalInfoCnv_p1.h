/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigOperationalInfoCnv_p1
 *
 * @brief transient persistent converter for TrigOperationalInfo
 *
 * @author Tomasz Bold (based on exampels by Andrew Hamilton and Francesca Bucci)
 *
 * File and Version Information:
 * $Id: TrigOperationalInfoCnv_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigOperationalInfoCnv_p1_H
#define TrigSteeringEventTPCnv_TrigOperationalInfoCnv_p1_H

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfo_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigOperationalInfoCnv_p1 : public T_AthenaPoolTPCnvBase<TrigOperationalInfo, TrigOperationalInfo_p1>
{
public:

  TrigOperationalInfoCnv_p1() {}

  virtual void persToTrans(const TrigOperationalInfo_p1* persObj, 
			   TrigOperationalInfo* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigOperationalInfo* transObj, 
			   TrigOperationalInfo_p1* persObj, 
			   MsgStream &log);
};

#endif
