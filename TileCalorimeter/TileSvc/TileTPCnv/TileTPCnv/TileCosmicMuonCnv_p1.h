///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p1.h 
// Header file for class TileCosmicMuonCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILECOSMICMUONCNV_P1_H
#define TILETPCNV_TILECOSMICMUONCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuon_p1.h"

// TileEvent includes
#include "TileEvent/TileCosmicMuon.h"

class MsgStream;

class TileCosmicMuonCnv_p1 : public T_AthenaPoolTPCnvBase<TileCosmicMuon, TileCosmicMuon_p1> {

public:

  /** Default constructor: 
   */
  TileCosmicMuonCnv_p1() {}

  /** Method creating the transient representation TileCosmicMuon
   *  from its persistent representation TileCosmicMuon_p1
   */
  virtual void persToTrans(const TileCosmicMuon_p1* persObj, TileCosmicMuon* transObj, MsgStream &log);

  /** Method creating the persistent representation TileCosmicMuon_p1
   *  from its transient representation TileCosmicMuon
   */
  virtual void transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p1* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILECOSMICMUONCNV_P1_H
