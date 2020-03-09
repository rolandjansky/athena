///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileL2Cnv_p1.h 
// Header file for class TileL2Cnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEL2CNV_P1_H
#define TILETPCNV_TILEL2CNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2_p1.h"

// TileEvent includes
#include "TileEvent/TileL2.h"

class MsgStream;

class TileL2Cnv_p1 : public T_AthenaPoolTPCnvConstBase<TileL2, TileL2_p1> {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  TileL2Cnv_p1() {}

  /** Method creating the transient representation TileL2
   *  from its persistent representation TileL2_p1
   */
  virtual void persToTrans(const TileL2_p1* persObj, TileL2* transObj, MsgStream &log) const override;

  /** Method creating the persistent representation TileL2_p1
   *  from its transient representation TileL2
   */
  virtual void transToPers(const TileL2* transObj, TileL2_p1* persObj, MsgStream &log) const override;

};

#endif //> TILETPCNV_TILEL2CNV_P1_H
