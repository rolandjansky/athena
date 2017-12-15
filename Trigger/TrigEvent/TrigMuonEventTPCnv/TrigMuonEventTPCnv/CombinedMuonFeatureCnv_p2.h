/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : CombinedMuonFeatureCnv_p1
 *
 * @brief persistent version of CombinedMuonFeatureCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureCnv_p2.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P2_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P2_H

#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "AthLinks/ElementLink.h"

class MsgStream;

class CombinedMuonFeatureCnv_p2  : public T_AthenaPoolTPCnvBase<CombinedMuonFeature, CombinedMuonFeature_p2>  {
    
  public:
    CombinedMuonFeatureCnv_p2() {}

    virtual void persToTrans(const CombinedMuonFeature_p2* persObj,  CombinedMuonFeature* transObj,   MsgStream &log );
    virtual void transToPers(const CombinedMuonFeature* transObj, CombinedMuonFeature_p2* persObj, MsgStream &log );

 protected:
 ElementLinkCnv_p1< ElementLink< MuonFeatureContainer > > m_elementLinkMuFeatCnv ;
 ElementLinkCnv_p1< ElementLink< TrigInDetTrackCollection > > m_elementLinkInDetTrackCnv ;

  };


#endif
