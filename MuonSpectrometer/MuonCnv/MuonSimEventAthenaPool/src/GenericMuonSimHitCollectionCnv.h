/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_GENERICMUONSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_GENERICMUONSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "MuonSimEventTPCnv/GenericMuonSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/GenericMuonSimHitCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::GenericMuonSimHitCollection_p1  GenericMuonSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<GenericMuonSimHitCollection, GenericMuonSimHitCollection_PERS >  GenericMuonSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class GenericMuonSimHitCollectionCnv : 
    public GenericMuonSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<GenericMuonSimHitCollectionCnv>;
    
public:
    GenericMuonSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~GenericMuonSimHitCollectionCnv();
    
    virtual GenericMuonSimHitCollection_PERS*   createPersistent (GenericMuonSimHitCollection* transCont);
    virtual GenericMuonSimHitCollection*        createTransient ();
        
private:
    GenericMuonSimHitCollectionCnv_p1    m_TPConverter_p1;
    GenericMuonSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
