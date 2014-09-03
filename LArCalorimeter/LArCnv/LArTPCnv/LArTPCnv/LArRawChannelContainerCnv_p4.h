/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class LArRawChannelContainerCnv_p4 : public T_AthenaPoolTPCnvBase<LArRawChannelContainer, LArRawChannelContainer_p4>
{
 public:
  LArRawChannelContainerCnv_p4() {};
  
  virtual void	persToTrans(const LArRawChannelContainer_p4* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) ;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p4* persColl,
			    MsgStream &log) ;
 private:
  LArRawChannelCnv_p2 m_larRawChannelCnv_p2;
};

#endif
