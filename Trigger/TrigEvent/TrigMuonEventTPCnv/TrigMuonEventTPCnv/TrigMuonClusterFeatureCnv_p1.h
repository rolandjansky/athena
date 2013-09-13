/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureCnv_p1
 *
 * @brief persistent version of TrigMuonClusterFeatureCnv
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureCnv_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"

class MsgStream;

class TrigMuonClusterFeatureCnv_p1  : public T_AthenaPoolTPCnvBase<TrigMuonClusterFeature, TrigMuonClusterFeature_p1>  {
    
  public:
    TrigMuonClusterFeatureCnv_p1() {}

    virtual void persToTrans(const TrigMuonClusterFeature_p1* persObj,  TrigMuonClusterFeature* transObj,   MsgStream &log );
    virtual void transToPers(const TrigMuonClusterFeature* transObj, TrigMuonClusterFeature_p1* persObj, MsgStream &log );

 protected:

};
#endif
