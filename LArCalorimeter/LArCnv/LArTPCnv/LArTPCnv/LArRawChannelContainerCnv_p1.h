/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNELCONTAINERCNV_P1_H
#define LARTPCNV_LARRAWCHANNELCONTAINERCNV_P1_H

// LArRawChannelContainerCnv_p1, T/P separation of LArRawChannel
// author R.Seuster <seuster@cern.ch>

#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArRawChannelContainer_p1.h"
//#include "LArRawChannelCnv_p1.h"
//#include "LArRawChannelCollectionCnv_p1.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"


//typedef  T_AthenaPoolTPCnvIDCont< LArRawChannelContainer, LArRawChannelContainer_p1, LArRawChannelCollectionCnv_p1 >	LArRawChannelContainer_Cnvp1_base_t;

class LArRawChannelContainerCnv_p1 : public T_AthenaPoolTPCnvConstBase<LArRawChannelContainer, LArRawChannelContainer_p1>
{
 public:
  LArRawChannelContainerCnv_p1() {};
  using base_class::persToTrans;
  using base_class::transToPers;
  
  virtual void	persToTrans(const LArRawChannelContainer_p1* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) const override;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p1* persColl,
			    MsgStream &log) const override;

 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;

};

#endif
