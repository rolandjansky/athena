///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileRawChannelCnv_p1.h 
// Transient/Persistent converter for TileRawChannel class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILERAWCHANNELCNV_P1_H
#define TILETPCNV_TILERAWCHANNELCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileRawChannel_p1.h"

// TileEvent includes
#include "TileEvent/TileRawChannel.h"

class MsgStream;

class TileRawChannelCnv_p1 : public T_AthenaPoolTPCnvBase<TileRawChannel, TileRawChannel_p1> {

public:

  /** Default constructor: 
   */
  TileRawChannelCnv_p1() {}

  /** Method creating the transient representation TileRawChannel
   *  from its persistent representation TileRawChannel_p1
   */
  virtual void persToTrans(const TileRawChannel_p1* persObj, TileRawChannel* transObj, MsgStream &log);

  /** Method creating the persistent representation TileRawChannel_p1
   *  from its transient representation TileRawChannel
   */
  virtual void transToPers(const TileRawChannel* transObj, TileRawChannel_p1* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILERAWCHANNELCNV_P1_H
