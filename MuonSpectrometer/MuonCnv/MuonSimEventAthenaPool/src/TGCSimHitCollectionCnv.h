/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_TGCSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_TGCSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/TGCSimHitCollection.h"

#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/TGCSimHitCollectionCnv_p4.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::TGCSimHitCollection_p4  TGCSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<TGCSimHitCollection, TGCSimHitCollection_PERS >  TGCSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class TGCSimHitCollectionCnv : 
    public TGCSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<TGCSimHitCollectionCnv>;
    
public:
    TGCSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~TGCSimHitCollectionCnv();
    
    virtual TGCSimHitCollection_PERS*   createPersistent (TGCSimHitCollection* transCont);
    virtual TGCSimHitCollection*        createTransient ();
        
private:
    TGCSimHitCollectionCnv_p1    m_TPConverter_p1;
    TGCSimHitCollectionCnv_p2    m_TPConverter_p2;
    TGCSimHitCollectionCnv_p3    m_TPConverter_p3;
    TGCSimHitCollectionCnv_p4    m_TPConverter_p4;
};

#endif
