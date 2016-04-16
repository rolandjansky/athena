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
  enum {PHI=0, ETA=1, ZED=2, WPHI=3, WETA=4, WZED=5, ETAPLUS=6, ETAMINUS=7};
  enum {ROIID=0, ROIWORD=1};
};

#endif
