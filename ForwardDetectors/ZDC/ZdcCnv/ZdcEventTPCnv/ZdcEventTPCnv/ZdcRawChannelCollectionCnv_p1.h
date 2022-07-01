///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ZdcRawChannelCollectionCnv_p1.h 
// Header file for class ZdcRawChannelCollectionCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef ZDCTPCNV_ZDCRAWCHANNELCOLLECTIONCNV_P1_H
#define ZDCTPCNV_ZDCRAWCHANNELCOLLECTIONCNV_P1_H

// ZdcEventTPCnv includes
#include "ZdcEventTPCnv/ZdcRawChannelCollection_p1.h"
#include "ZdcEventTPCnv/ZdcRawChannelCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Zdc includes
#include "ZdcEvent/ZdcRawChannelCollection.h"

class MsgStream;

class ZdcRawChannelCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ZdcRawChannelCollection, ZdcRawChannelCollection_p1> {
  
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  ZdcRawChannelCollectionCnv_p1() {}
  
  virtual void persToTrans(const ZdcRawChannelCollection_p1* persObj, ZdcRawChannelCollection*   transObj, MsgStream& log) const override;
  virtual void transToPers(const ZdcRawChannelCollection*   transObj, ZdcRawChannelCollection_p1* persObj, MsgStream& log) const override;
};


template<>
class T_TPCnv<ZdcRawChannelCollection, ZdcRawChannelCollection_p1>
  : public ZdcRawChannelCollectionCnv_p1
{
public:
};

#endif //> ZDCTPCNV_ZDCRAWCHANNELCOLLECTIONCNV_P1_H

