/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureCnv_p2
 *
 * @brief transient-persistent converter for TileMuFeature
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Apr. 16. 2008
 **********************************************************************************/

#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURECNV_P2_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURECNV_P2_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TileMuFeature transient and persistent classes
#include "TrigMuonEventTPCnv/TileMuFeature_p2.h"
#include "TrigMuonEvent/TileMuFeature.h"

class MsgStream;

class TileMuFeatureCnv_p2 : public T_AthenaPoolTPCnvBase<TileMuFeature, TileMuFeature_p2> {

 public:

  TileMuFeatureCnv_p2() {}

  virtual void persToTrans(const TileMuFeature_p2* persObj, TileMuFeature* transObj, MsgStream &log);
  virtual void transToPers(const TileMuFeature* transObj, TileMuFeature_p2* persObj, MsgStream &log);

 protected:

};
#endif

