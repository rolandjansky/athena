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

  enum {PHI=0, ETA=1, ZED=2, 
	PHIPLUS=3, PHIMINUS=4, 
	ETAPLUS=5, ETAMINUS=6, 
	ZEDPLUS=7, ZEDMINUS=8};

  enum {ROIID=0, ROIWORD=1};

};

#endif
