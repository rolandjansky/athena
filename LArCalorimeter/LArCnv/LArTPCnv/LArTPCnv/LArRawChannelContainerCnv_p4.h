/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINERCNV_P4_H
#define LARTPCNV_LARRAWCHANNELCONTAINERCNV_P4_H

// LArRawChannelContainerCnv_p4, T/P separation of LArRawChannel
// based on  LArRawChannelContainerCnv_p3, using new LArRawChannel_p2
// author G.Unal

#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArRawChannelContainer_p4.h"
#include "LArTPCnv/LArRawChannelCnv_p2.h"

class LArRawChannelContainerCnv_p4 : public T_AthenaPoolTPCnvConstBase<LArRawChannelContainer, LArRawChannelContainer_p4>
{
 public:
  LArRawChannelContainerCnv_p4() {};
  using base_class::persToTrans;
  using base_class::transToPers;
  
  virtual void	persToTrans(const LArRawChannelContainer_p4* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) const override;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p4* persColl,
			    MsgStream &log) const override;
 private:
  LArRawChannelCnv_p2 m_larRawChannelCnv_p2;
};

#endif
