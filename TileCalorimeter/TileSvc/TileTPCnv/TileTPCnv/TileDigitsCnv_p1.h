///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsCnv_p1.h 
// Transient/Persistent converter for TileDigits class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITSCNV_P1_H
#define TILETPCNV_TILEDIGITSCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileDigits_p1.h"

// TileEvent includes
#include "TileEvent/TileDigits.h"

class MsgStream;

class TileDigitsCnv_p1 : public T_AthenaPoolTPCnvBase<TileDigits, TileDigits_p1> {

public:

  /** Default constructor: 
   */
  TileDigitsCnv_p1() {}

  /** Method creating the transient representation TileDigits
   *  from its persistent representation TileDigits_p1
   */
  virtual void persToTrans(const TileDigits_p1* persObj, TileDigits* transObj, MsgStream &log);

  /** Method creating the persistent representation TileDigits_p1
   *  from its transient representation TileDigits
   */
  virtual void transToPers(const TileDigits* transObj, TileDigits_p1* persObj, MsgStream &log);

};

#endif //> TILETPCNV_TILEDIGITSCNV_P1_H
