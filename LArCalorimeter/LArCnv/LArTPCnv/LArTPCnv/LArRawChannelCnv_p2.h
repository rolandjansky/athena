/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARRAWCHANNELCNV_P2_H
#define LARTPCNV_LARRAWCHANNELCNV_P2_H

#include "LArTPCnv/LArRawChannel_p2.h"
#include "LArRawEvent/LArRawChannel.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// LArRawChannelCnv_p2, used for T/P separation
// author G.Unal

class MsgStream;
class LArRawChannelCnv_p2  : public T_AthenaPoolTPCnvBase<LArRawChannel, LArRawChannel_p2>
{
 public:
  LArRawChannelCnv_p2() { };
  virtual void   persToTrans(const LArRawChannel_p2* pers, LArRawChannel* trans, MsgStream &log) ;
  virtual void   transToPers(const LArRawChannel* trans, LArRawChannel_p2* pers, MsgStream &log) ;

};

#endif
