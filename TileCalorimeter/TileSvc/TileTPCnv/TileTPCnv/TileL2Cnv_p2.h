///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p2.h 
// Header file for class TileL2Cnv_p2
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEL2CNV_P2_H
#define TILETPCNV_TILEL2CNV_P2_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2_p2.h"

// TileEvent includes
#include "TileEvent/TileL2.h"

class MsgStream;

class TileL2Cnv_p2 : public T_AthenaPoolTPCnvBase<TileL2, TileL2_p2> {

public:

  /** Default constructor: 
   */
  TileL2Cnv_p2() {}

  /** Method creating the transient representation TileL2
   *  from its persistent representation TileL2_p2
   */
  virtual void persToTrans(const TileL2_p2* persObj, TileL2* transObj, MsgStream &log);

  /** Method creating the persistent representation TileL2_p2
   *  from its transient representation TileL2
   */
  virtual void transToPers(const TileL2* transObj, TileL2_p2* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILEL2CNV_P2_H
