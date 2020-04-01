/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_RPCSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_RPCSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/RPCSimHitCollection.h"

#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p3.h"
#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p4.h"


// the latest persistent representation type of DataCollection:
typedef  Muon::RPCSimHitCollection_p4  RPCSimHitCollection_PERS;
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
    RPCSimHitCollectionCnv_p3    m_TPConverter_p3;
    RPCSimHitCollectionCnv_p4    m_TPConverter_p4;
};

#endif
