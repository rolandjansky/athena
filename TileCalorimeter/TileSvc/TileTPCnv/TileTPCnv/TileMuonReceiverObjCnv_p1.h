///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverObjCnv_p1.h 
// Header file for class TileMuonReceiverObjCnv_p1
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMUONRECEIVEROBJCNV_P1_H
#define TILETPCNV_TILEMUONRECEIVEROBJCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverObj_p1.h"

// TileEvent includes
#include "TileEvent/TileMuonReceiverObj.h"

class MsgStream;

class TileMuonReceiverObjCnv_p1 : public T_AthenaPoolTPCnvBase<TileMuonReceiverObj, TileMuonReceiverObj_p1> {

public:

  /** Default constructor: 
   */
  TileMuonReceiverObjCnv_p1() {}

  /** Method creating the transient representation TileMuonReceiverObj
   *  from its persistent representation TileMuonReceiverObj_p1
   */
  virtual void persToTrans(const TileMuonReceiverObj_p1* persObj, TileMuonReceiverObj* transObj, MsgStream &log);

  /** Method creating the persistent representation TileMuonReceiverObj_p1
   *  from its transient representation TileMuonReceiverObj
   */
  virtual void transToPers(const TileMuonReceiverObj* transObj, TileMuonReceiverObj_p1* persObj, MsgStream &log);

};

#endif
