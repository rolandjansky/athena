/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureCnv_p2
 *
 * @brief persistent version of IsoMuonFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureCnv_p2.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_P2_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEventTPCnv/IsoMuonFeature_p2.h"

class MsgStream;

class IsoMuonFeatureCnv_p2  : public T_AthenaPoolTPCnvBase<IsoMuonFeature, IsoMuonFeature_p2>  {
    
  public:
    IsoMuonFeatureCnv_p2() {}

    virtual void persToTrans(const IsoMuonFeature_p2* persObj,  IsoMuonFeature* transObj,   MsgStream &log );
    virtual void transToPers(const IsoMuonFeature* transObj, IsoMuonFeature_p2* persObj, MsgStream &log );

 protected:

};
#endif
