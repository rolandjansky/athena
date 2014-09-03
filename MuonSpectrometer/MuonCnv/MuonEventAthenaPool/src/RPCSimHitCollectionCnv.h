/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCSIMHITCOLLECTIONCNV_H
#define MUONEVENTATHENAPOOL_RPCSIMHITCOLLECTIONCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/RPCSimHitCollection.h"


#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonSimEvent/RPCSimHitCollectionCnv_p2.h"


// the latest persistent representation type of DataCollection:
typedef  Muon::RPCSimHitCollection_p2  RPCSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<RPCSimHitCollection, RPCSimHitCollection_PERS >  RPCSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class RPCSimHitCollectionCnv : 
    public RPCSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<RPCSimHitCollectionCnv>;
    
public:
    RPCSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~RPCSimHitCollectionCnv();
    
    virtual RPCSimHitCollection_PERS*   createPersistent (RPCSimHitCollection* transCont);
    virtual RPCSimHitCollection*        createTransient ();
        
private:
    RPCSimHitCollectionCnv_p1    m_TPConverter;
    RPCSimHitCollectionCnv_p2    m_TPConverter_p2;
};

#endif
