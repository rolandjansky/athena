/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class LArRawChannelContainerCnv_p1 : public T_AthenaPoolTPCnvBase<LArRawChannelContainer, LArRawChannelContainer_p1>
{
 public:
  LArRawChannelContainerCnv_p1() {};
  
  virtual void	persToTrans(const LArRawChannelContainer_p1* persColl,
			    LArRawChannelContainer* transColl,
			    MsgStream &log) ;
  virtual void	transToPers(const LArRawChannelContainer* transColl,
			    LArRawChannelContainer_p1* persColl,
			    MsgStream &log) ;

 private:
  LArRawChannelCnv_p1 m_larRawChannelCnv_p1;

};

#endif
