/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_STGCSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_STGCSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"

#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p2.h"

/**
 ** The _p2 converter was added to support HepMcParticleLink_p2 in 21.3.
 ** As of 2018-09-24, HepMcParticleLink_p2 is not fully supported in master.
 ** Therefore, the default is set to _p2, while the _p2 is provided
 ** in case of it is needed in the future.
 ** -chav.chhiv.Chau@cern.ch
 **/

// the latest persistent representation type of DataCollection:
typedef  Muon::sTGCSimHitCollection_p1  sTGCSimHitCollection_PERS;
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
    sTGCSimHitCollectionCnv_p1    m_TPConverter_p1;
    sTGCSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
