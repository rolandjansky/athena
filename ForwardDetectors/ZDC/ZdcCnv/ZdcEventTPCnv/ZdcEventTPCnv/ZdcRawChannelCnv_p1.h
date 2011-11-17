///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannelCnv_p1.h 
// Transient/Persistent converter for ZdcRawChannel class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCTPCNV_ZDCRAWCHANNELCNV_P1_H
#define ZDCTPCNV_ZDCRAWCHANNELCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ZdcEventTPCnv includes
#include "ZdcEventTPCnv/ZdcRawChannel_p1.h"

// ZdcEvent includes
#include "ZdcEvent/ZdcRawChannel.h"

class MsgStream;

class ZdcRawChannelCnv_p1 : public T_AthenaPoolTPCnvBase<ZdcRawChannel, ZdcRawChannel_p1> {

public:

  /** Default constructor: 
   */
  ZdcRawChannelCnv_p1() {}

  /** Method creating the transient representation ZdcRawChannel
   *  from its persistent representation ZdcRawChannel_p1
   */
  virtual void persToTrans(const ZdcRawChannel_p1* persObj, ZdcRawChannel* transObj, MsgStream &log);

  /** Method creating the persistent representation ZdcRawChannel_p1
   *  from its transient representation ZdcRawChannel
   */
  virtual void transToPers(const ZdcRawChannel* transObj, ZdcRawChannel_p1* persObj, MsgStream &log);

};

#endif //> ZDCTPCNV_ZDCRAWCHANNELCNV_P1_H
