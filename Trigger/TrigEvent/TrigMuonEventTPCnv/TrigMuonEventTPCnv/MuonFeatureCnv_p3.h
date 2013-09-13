/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureCnv_p3
 *
 * @brief persistent version of MuonFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: MuonFeatureCnv_p3.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_MUONFEATURECNV_P3_H
#define TRIGMUONEVENTTPCNV_MUONFEATURECNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEventTPCnv/MuonFeature_p3.h"

class MsgStream;

class MuonFeatureCnv_p3  : public T_AthenaPoolTPCnvBase<MuonFeature, MuonFeature_p3>  {
    
  public:
    MuonFeatureCnv_p3() {}

    virtual void persToTrans(const MuonFeature_p3* persObj,  MuonFeature* transObj,   MsgStream &log );
    virtual void transToPers(const MuonFeature* transObj, MuonFeature_p3* persObj, MsgStream &log );

 protected:

};
#endif
