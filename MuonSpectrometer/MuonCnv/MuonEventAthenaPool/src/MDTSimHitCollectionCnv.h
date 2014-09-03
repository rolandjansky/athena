/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MDTSIMHITCOLLECTIONCNV_H
#define MUONEVENTATHENAPOOL_MDTSIMHITCOLLECTIONCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/MDTSimHitCollection.h"


#include "MuonEventTPCnv/MuonSimEvent/MDTSimHitCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonSimEvent/MDTSimHitCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::MDTSimHitCollection_p2  MDTSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<MDTSimHitCollection, MDTSimHitCollection_PERS >  MDTSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MDTSimHitCollectionCnv : 
    public MDTSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<MDTSimHitCollectionCnv>;
    
public:
    MDTSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~MDTSimHitCollectionCnv();
    
    virtual MDTSimHitCollection_PERS*   createPersistent (MDTSimHitCollection* transCont);
    virtual MDTSimHitCollection*        createTransient ();
        
private:
    MDTSimHitCollectionCnv_p1    m_TPConverter_p1;
    MDTSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
