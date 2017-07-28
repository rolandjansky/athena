/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMEVENTATHENAPOOL_CSCSIMHITCOLLECTIONCNV_H
#define MUONSIMEVENTATHENAPOOL_CSCSIMHITCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimEvent/CSCSimHitCollection.h"

#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p1.h"
#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p2.h"
#include "MuonSimEventTPCnv/CSCSimHitCollectionCnv_p3.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::CSCSimHitCollection_p3  CSCSimHitCollection_PERS;
typedef  T_AthenaPoolCustomCnv<CSCSimHitCollection, CSCSimHitCollection_PERS >  CSCSimHitCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class CSCSimHitCollectionCnv : 
    public CSCSimHitCollectionCnvBase 
{
    
    friend class CnvFactory<CSCSimHitCollectionCnv>;
    
public:
    CSCSimHitCollectionCnv(ISvcLocator* svcloc);
    virtual ~CSCSimHitCollectionCnv();
    
    virtual CSCSimHitCollection_PERS*   createPersistent (CSCSimHitCollection* transCont);
    virtual CSCSimHitCollection*        createTransient ();

private:
    CSCSimHitCollectionCnv_p1    m_TPConverter;
    CSCSimHitCollectionCnv_p2    m_TPConverter_p2;
    CSCSimHitCollectionCnv_p3    m_TPConverter_p3;
};

#endif
