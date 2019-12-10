///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileMuCnv_p1.h 
// Header file for class TileMuCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMUCNV_P1_H
#define TILETPCNV_TILEMUCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileMu_p1.h"

// TileEvent includes
#include "TileEvent/TileMu.h"

class MsgStream;

class TileMuCnv_p1 : public T_AthenaPoolTPCnvConstBase<TileMu, TileMu_p1> {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  TileMuCnv_p1() {}

  /** Method creating the transient representation TileMu
   *  from its persistent representation TileMu_p1
   */
  virtual void persToTrans(const TileMu_p1* persObj, TileMu* transObj, MsgStream &log) const override;

  /** Method creating the persistent representation TileMu_p1
   *  from its transient representation TileMu
   */
  virtual void transToPers(const TileMu* transObj, TileMu_p1* persObj, MsgStream &log) const override;

};

#endif //> TILETPCNV_TILEMUCNV_P1_H
