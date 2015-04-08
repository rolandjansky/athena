/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCnv_p3
 *
 * @brief transient persistent converter for TrigRoiDescriptor
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p3_H
#define TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p3_H

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigRoiDescriptorCnv_p3 : public T_AthenaPoolTPCnvBase<TrigRoiDescriptor, TrigRoiDescriptor_p3>
{
public:

  TrigRoiDescriptorCnv_p3() {}

  virtual void persToTrans(const TrigRoiDescriptor_p3* persObj, 
			   TrigRoiDescriptor* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigRoiDescriptor* transObj, 
			   TrigRoiDescriptor_p3* persObj, 
			   MsgStream &log);

  enum {phi=0, eta=1, zed=2, 
	phiplus=3, phiminus=4, 
	etaplus=5, etaminus=6, 
	zedplus=7, zedminus=8};

  enum {roiid=0, roiword=1};

};

#endif
