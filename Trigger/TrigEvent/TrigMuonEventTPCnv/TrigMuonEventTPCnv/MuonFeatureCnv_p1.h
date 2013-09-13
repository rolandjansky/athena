/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureCnv_p1
 *
 * @brief transient-persistent converter for MuonFeature
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECNV_P1_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//MuonFeature transient and persistent classes
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"


class MsgStream;


class MuonFeatureCnv_p1 : public T_AthenaPoolTPCnvBase<MuonFeature, MuonFeature_p1> 
{

 public:

  MuonFeatureCnv_p1() {}

  virtual void persToTrans(const MuonFeature_p1* persObj, MuonFeature* transObj, MsgStream &log);
  virtual void transToPers(const MuonFeature* transObj, MuonFeature_p1* persObj, MsgStream &log);
};

#endif
