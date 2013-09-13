/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureCnv_p3
 *
 * @brief persistent version of IsoMuonFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureCnv_p3.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_P3_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p3.h"

class MsgStream;

class IsoMuonFeatureCnv_p3  : public T_AthenaPoolTPCnvBase<IsoMuonFeature, IsoMuonFeature_p3>  {
    
  public:
    IsoMuonFeatureCnv_p3() {}

    virtual void persToTrans(const IsoMuonFeature_p3* persObj,  IsoMuonFeature* transObj,   MsgStream &log );
    virtual void transToPers(const IsoMuonFeature* transObj, IsoMuonFeature_p3* persObj, MsgStream &log );

 protected:

};
#endif
