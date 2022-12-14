///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonCnv_p2.h 
// Header file for class TileCosmicMuonCnv_p2
// Author: Jose Maneira <maneira@lip.pt>
// Date:   July 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILECOSMICMUONCNV_P2_H
#define TILETPCNV_TILECOSMICMUONCNV_P2_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuon_p2.h"

// TileEvent includes
#include "TileEvent/TileCosmicMuon.h"

class MsgStream;

class TileCosmicMuonCnv_p2 : public T_AthenaPoolTPCnvConstBase<TileCosmicMuon, TileCosmicMuon_p2> {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  TileCosmicMuonCnv_p2() {}

  /** Method creating the transient representation TileCosmicMuon
   *  from its persistent representation TileCosmicMuon_p2
   */
  virtual void persToTrans(const TileCosmicMuon_p2* persObj, TileCosmicMuon* transObj, MsgStream &log) const override;

  /** Method creating the persistent representation TileCosmicMuon_p2
   *  from its transient representation TileCosmicMuon
   */
  virtual void transToPers(const TileCosmicMuon* transObj, TileCosmicMuon_p2* persObj, MsgStream &log) const override;

};

#endif //> TILETPCNV_TILECOSMICMUONCNV_P2_H
