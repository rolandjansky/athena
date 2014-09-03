/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINERCNV_P2_H
#define LARTPCNV_LARRAWCHANNELCONTAINERCNV_P2_H

// LArRawChannelContainerCnv_p2, T/P separation of LArRawChannel
// author R.Seuster <seuster@cern.ch>

#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArRawChannelContainer_p2.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"

// We do NOT use T_AthenaPoolTPCnvIDCont because we do all of the
// conversion in the .cxx. In part because LArRawChannelContainer_p2
// vecs of both collections and channels. As well the IDC
// addCollection is better.
class LArRawChannelContainerCnv_p2 : public T_AthenaPoolTPCnvBase<LArRawChannelContainer, LArRawChannelContainer_p2>
{
 public:
  LArRawChannelContainerCnv_p2() {};
  
  virtual void	persToTrans(const LArRawChannelContainer_p2* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) ;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p2* persColl,
			    MsgStream &log) ;
 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;
};

#endif
