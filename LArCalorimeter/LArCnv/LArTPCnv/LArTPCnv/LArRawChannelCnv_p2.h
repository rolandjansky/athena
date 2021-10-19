/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARRAWCHANNELCNV_P2_H
#define LARTPCNV_LARRAWCHANNELCNV_P2_H

#include "LArTPCnv/LArRawChannel_p2.h"
#include "LArRawEvent/LArRawChannel.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// LArRawChannelCnv_p2, used for T/P separation
// author G.Unal

class MsgStream;
class LArRawChannelCnv_p2  : public T_AthenaPoolTPCnvConstBase<LArRawChannel, LArRawChannel_p2>
{
 public:
  LArRawChannelCnv_p2() { };
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const LArRawChannel_p2* pers, LArRawChannel* trans, MsgStream &log) const override;
  virtual void   transToPers(const LArRawChannel* trans, LArRawChannel_p2* pers, MsgStream &log) const override;

};

#endif
