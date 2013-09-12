/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuonCnv_p1
 *
 * @brief transient-persistent converter for CosmicMuon_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCnv_p1.h,v 1.2 2009-04-01 22:02:51 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_COSMICMUONCNV_P1_H
#define TRIGCOSMICEVENTTPCNV_COSMICMUONCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//CosmicMuon persistent class
#include "TrigCosmicEventTPCnv/CosmicMuon_p1.h"


class MsgStream;


class CosmicMuonCnv_p1 : public T_AthenaPoolTPCnvBase<CosmicMuon, CosmicMuon_p1> 
{

 public:

  CosmicMuonCnv_p1() {}

  virtual void persToTrans(const CosmicMuon_p1* persObj, CosmicMuon* transObj, MsgStream &log);
  virtual void transToPers(const CosmicMuon* transObj, CosmicMuon_p1* persObj, MsgStream &log);
 
};

#endif
