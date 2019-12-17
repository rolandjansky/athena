///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1Cnv_p1.h 
// Transient/Persistent converter for TileTTL1 class
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1CNV_P1_H
#define TILETPCNV_TILETTL1CNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1_p1.h"

// TileEvent includes
#include "TileEvent/TileTTL1.h"

class MsgStream;

class TileTTL1Cnv_p1 : public T_AthenaPoolTPCnvConstBase<TileTTL1, TileTTL1_p1> {

public:
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  TileTTL1Cnv_p1() {}

  /** Method creating the transient representation TileTTL1
   *  from its persistent representation TileTTL1_p1
   */
  virtual void persToTrans(const TileTTL1_p1* persObj, TileTTL1* transObj, MsgStream &log) const override;

  /** Method creating the persistent representation TileTTL1_p1
   *  from its transient representation TileTTL1
   */
  virtual void transToPers(const TileTTL1* transObj, TileTTL1_p1* persObj, MsgStream &log) const override;

};

#endif //> TILETPCNV_TILETTL1CNV_P1_H
