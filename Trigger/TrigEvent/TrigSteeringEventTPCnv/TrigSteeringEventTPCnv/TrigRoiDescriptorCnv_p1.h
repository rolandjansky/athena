/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigRoiDescriptorCnv_p1
 *
 * @brief transient persistent converter for TrigRoiDescriptor
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigRoiDescriptorCnv_p1.h,v 1.2 2009-04-01 22:14:57 salvator Exp $
 **********************************************************************************/
#ifndef TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p1_H
#define TrigSteeringEventTPCnv_TrigRoiDescriptorCnv_p1_H

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigRoiDescriptorCnv_p1 : public T_AthenaPoolTPCnvBase<TrigRoiDescriptor, TrigRoiDescriptor_p1>
{
public:

  TrigRoiDescriptorCnv_p1() {}

  virtual void persToTrans(const TrigRoiDescriptor_p1* persObj, 
			   TrigRoiDescriptor* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigRoiDescriptor* transObj, 
			   TrigRoiDescriptor_p1* persObj, 
			   MsgStream &log);
};


#endif
