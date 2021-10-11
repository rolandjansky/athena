/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
class LArRawChannelContainerCnv_p2 : public T_AthenaPoolTPCnvConstBase<LArRawChannelContainer, LArRawChannelContainer_p2>
{
 public:
  LArRawChannelContainerCnv_p2() {};
  using base_class::persToTrans;
  using base_class::transToPers;
  
  virtual void	persToTrans(const LArRawChannelContainer_p2* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) const override;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p2* persColl,
			    MsgStream &log) const override;
 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;
};

#endif
