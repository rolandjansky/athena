/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_STGCSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_STGCSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"

#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::sTGCSimHitCollection_p2  sTGCSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<sTGCSimHitCollection, sTGCSimHitCollection_PERS >  sTGCSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class sTGCSimHitCollectionCnv : 
    public sTGCSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<sTGCSimHitCollectionCnv>;
    
public:
    sTGCSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~sTGCSimHitCollectionCnv();
    
    virtual sTGCSimHitCollection_PERS*   createPersistent (sTGCSimHitCollection* transCont);
    virtual sTGCSimHitCollection*        createTransient ();
        
private:
    sTGCSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
