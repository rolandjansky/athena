/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARRAWCHANNELCNV_P1_H
#define LARTPCNV_LARRAWCHANNELCNV_P1_H

#include "LArTPCnv/LArRawChannel_p1.h"
#include "LArRawEvent/LArRawChannel.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// LArRawChannelCnv_p1, used for T/P separation
// author R.Seuster <seuster@cern.ch>

class MsgStream;
class LArRawChannelCnv_p1  : public T_AthenaPoolTPCnvConstBase<LArRawChannel, LArRawChannel_p1>
{
 public:
  LArRawChannelCnv_p1() { };
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const LArRawChannel_p1* pers, LArRawChannel* trans, MsgStream &log) const override;
  virtual void   transToPers(const LArRawChannel* trans, LArRawChannel_p1* pers, MsgStream &log) const override;

};

#endif
