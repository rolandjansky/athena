/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINERCNV_P3_H
#define LARTPCNV_LARRAWCHANNELCONTAINERCNV_P3_H

// LArRawChannelContainerCnv_p3, T/P separation of LArRawChannel
// author Walter Lampl

#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArRawChannelContainer_p3.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"

class LArRawChannelContainerCnv_p3 : public T_AthenaPoolTPCnvConstBase<LArRawChannelContainer, LArRawChannelContainer_p3>
{
 public:
  LArRawChannelContainerCnv_p3() {};
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void	persToTrans(const LArRawChannelContainer_p3* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) const override;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p3* persColl,
			    MsgStream &log) const override;
 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;
};

#endif
