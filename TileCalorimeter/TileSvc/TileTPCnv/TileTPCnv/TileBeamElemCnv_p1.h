///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileBeamElemCnv_p1.h 
// Transient/Persistent converter for TileBeamElem class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEBEAMELEMCNV_P1_H
#define TILETPCNV_TILEBEAMELEMCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileBeamElem_p1.h"

// TileEvent includes
#include "TileEvent/TileBeamElem.h"

class MsgStream;

class TileBeamElemCnv_p1 : public T_AthenaPoolTPCnvBase<TileBeamElem, TileBeamElem_p1> {

public:

  /** Default constructor: 
   */
  TileBeamElemCnv_p1() {}

  /** Method creating the transient representation TileBeamElem
   *  from its persistent representation TileBeamElem_p1
   */
  virtual void persToTrans(const TileBeamElem_p1* persObj, TileBeamElem* transObj, MsgStream &log);

  /** Method creating the persistent representation TileBeamElem_p1
   *  from its transient representation TileBeamElem
   */
  virtual void transToPers(const TileBeamElem* transObj, TileBeamElem_p1* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILEBEAMELEMCNV_P1_H
