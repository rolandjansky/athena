/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINERCNV_P3_H
#define LARTPCNV_LARRAWCHANNELCONTAINERCNV_P3_H

// LArRawChannelContainerCnv_p3, T/P separation of LArRawChannel
// author Walter Lampl

#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArRawChannelContainer_p3.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"

class LArRawChannelContainerCnv_p3 : public T_AthenaPoolTPCnvBase<LArRawChannelContainer, LArRawChannelContainer_p3>
{
 public:
  LArRawChannelContainerCnv_p3() {};
  
  virtual void	persToTrans(const LArRawChannelContainer_p3* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) ;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p3* persColl,
			    MsgStream &log) ;
 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;
};

#endif
