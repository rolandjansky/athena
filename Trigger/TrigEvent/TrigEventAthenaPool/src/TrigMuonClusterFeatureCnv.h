/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMUONCLUSTERFEATURECNV_H
#define TRIGEVENTATHENAPOOL_TRIGMUONCLUSTERFEATURECNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_tlp1.h"
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigMuonClusterFeature_tlp1   TrigMuonClusterFeature_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMuonClusterFeature, TrigMuonClusterFeature_PERS> TrigMuonClusterFeatureCnvBase;
 
class TrigMuonClusterFeatureCnv : public TrigMuonClusterFeatureCnvBase {

 friend class CnvFactory<TrigMuonClusterFeatureCnv>;
 
 protected:

public:
 TrigMuonClusterFeatureCnv( ISvcLocator *svcloc ): TrigMuonClusterFeatureCnvBase(svcloc){}
protected:
    
   virtual TrigMuonClusterFeature_PERS* createPersistent( TrigMuonClusterFeature *transObj);
   virtual TrigMuonClusterFeature* createTransient();
private:
   TrigMuonClusterFeatureCnv_tlp1 m_converter;

 };
 
#endif // TRIGMUONCLUSTERFEATURE_CNV_H
