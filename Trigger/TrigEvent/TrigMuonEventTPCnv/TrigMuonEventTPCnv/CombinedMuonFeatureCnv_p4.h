/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : CombinedMuonFeatureCnv_p4
 *
 * @brief persistent version of CombinedMuonFeatureCnv
 *
 * @author Stefano Giagu    <Stefano.Giagu>            - U. Rome
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureCnv_p4.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P4_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P4_H

#include "TrigMuonEventTPCnv/CombinedMuonFeature_p4.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "AthLinks/ElementLink.h"

class MsgStream;

class CombinedMuonFeatureCnv_p4  : public T_AthenaPoolTPCnvBase<CombinedMuonFeature, CombinedMuonFeature_p4>  {
    
  public:
    CombinedMuonFeatureCnv_p4() {}

    virtual void persToTrans(const CombinedMuonFeature_p4* persObj,  CombinedMuonFeature* transObj,   MsgStream &log 
);
    virtual void transToPers(const CombinedMuonFeature* transObj, CombinedMuonFeature_p4* persObj, MsgStream &log );

 protected:
 ElementLinkCnv_p3< ElementLink< MuonFeatureContainer > > m_elementLinkMuFeatCnv ;
 ElementLinkCnv_p3< ElementLink< TrigInDetTrackCollection > > m_elementLinkInDetTrackCnv ;

  };


#endif
