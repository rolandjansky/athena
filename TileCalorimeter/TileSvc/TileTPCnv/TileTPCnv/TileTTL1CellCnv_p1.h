///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1CellCnv_p1.h 
// Transient/Persistent converter for TileTTL1Cell class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1CELLCNV_P1_H
#define TILETPCNV_TILETTL1CELLCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1Cell_p1.h"

// TileEvent includes
#include "TileEvent/TileTTL1Cell.h"

class MsgStream;

class TileTTL1CellCnv_p1 : public T_AthenaPoolTPCnvBase<TileTTL1Cell, TileTTL1Cell_p1> {

public:

  /** Default constructor: 
   */
  TileTTL1CellCnv_p1() {}

  /** Method creating the transient representation TileTTL1Cell
   *  from its persistent representation TileTTL1Cell_p1
   */
  virtual void persToTrans(const TileTTL1Cell_p1* persObj, TileTTL1Cell* transObj, MsgStream &log);

  /** Method creating the persistent representation TileTTL1Cell_p1
   *  from its transient representation TileTTL1Cell
   */
  virtual void transToPers(const TileTTL1Cell* transObj, TileTTL1Cell_p1* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILETTL1CELLCNV_P1_H
