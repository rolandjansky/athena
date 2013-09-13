/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureCnv_p1
 *
 * @brief transient-persistent converter for TileMuFeature
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Apr. 16. 2008
 **********************************************************************************/

#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURECNV_P1_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURECNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TileMuFeature transient and persistent classes
#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"
#include "TrigMuonEvent/TileMuFeature.h"

class MsgStream;

class TileMuFeatureCnv_p1 : public T_AthenaPoolTPCnvBase<TileMuFeature, TileMuFeature_p1>
{

 public:

  TileMuFeatureCnv_p1() {}

  virtual void persToTrans(const TileMuFeature_p1* persObj, TileMuFeature* transObj, MsgStream &log);
  virtual void transToPers(const TileMuFeature* transObj, TileMuFeature_p1* persObj, MsgStream &log);
};

#endif

