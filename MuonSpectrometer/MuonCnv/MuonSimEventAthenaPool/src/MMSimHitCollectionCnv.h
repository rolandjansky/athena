/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_MMSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_MMSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/MMSimHitCollection.h"

#include "MuonSimEventTPCnv/MMSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/MMSimHitCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::MMSimHitCollection_p2  MMSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<MMSimHitCollection, MMSimHitCollection_PERS >  MMSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MMSimHitCollectionCnv : 
    public MMSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<MMSimHitCollectionCnv>;
    
public:
    MMSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~MMSimHitCollectionCnv();
    
    virtual MMSimHitCollection_PERS*   createPersistent (MMSimHitCollection* transCont);
    virtual MMSimHitCollection*        createTransient ();
        
private:
    MMSimHitCollectionCnv_p1    m_TPConverter_p1;
    MMSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
