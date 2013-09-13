/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureCnv_p1
 *
 * @brief persistent version of CombinedMuonFeatureCnv
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"

class MsgStream;

class CombinedMuonFeatureCnv_p1  : public T_AthenaPoolTPCnvBase<CombinedMuonFeature, CombinedMuonFeature_p1>  {
    
  public:
    CombinedMuonFeatureCnv_p1() : m_mfCnv(0), m_IDTkCnv(0) {}

    virtual void persToTrans(const CombinedMuonFeature_p1* persObj,  CombinedMuonFeature* transObj,   MsgStream &log );
    virtual void transToPers(const CombinedMuonFeature* transObj, CombinedMuonFeature_p1* persObj, MsgStream &log );

 protected:
    MuonFeatureCnv_p1*      m_mfCnv;
    TrigInDetTrackCnv_p1*   m_IDTkCnv;

  };


#endif
