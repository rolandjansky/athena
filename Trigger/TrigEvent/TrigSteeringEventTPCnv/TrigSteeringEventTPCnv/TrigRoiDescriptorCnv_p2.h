/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCnv_p2
 *
 * @brief transient persistent converter for TrigRoiDescriptor
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p2_H
#define TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p2_H

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigRoiDescriptorCnv_p2 : public T_AthenaPoolTPCnvBase<TrigRoiDescriptor, TrigRoiDescriptor_p2>
{
public:

  TrigRoiDescriptorCnv_p2() {}

  virtual void persToTrans(const TrigRoiDescriptor_p2* persObj, 
			   TrigRoiDescriptor* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigRoiDescriptor* transObj, 
			   TrigRoiDescriptor_p2* persObj, 
			   MsgStream &log);
  enum {phi=0, eta=1, zed=2, wphi=3, weta=4, wzed=5, etaplus=6, etaminus=7};
  enum {roiid=0, roiword=1};
};

#endif
