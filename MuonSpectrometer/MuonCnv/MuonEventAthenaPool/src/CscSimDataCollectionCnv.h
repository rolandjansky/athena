/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_CSCSIMDATACOLLECTIONCNV_H
#define MUONEVENTATHENAPOOL_CSCSIMDATACOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::CscSimDataCollection_p2  CscSimDataCollection_PERS;
typedef  T_AthenaPoolCustomCnv<CscSimDataCollection,CscSimDataCollection_PERS >  CscSimDataCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class CscSimDataCollectionCnv : public CscSimDataCollectionCnvBase
{

    friend class CnvFactory<CscSimDataCollectionCnv>;

public:
    CscSimDataCollectionCnv(ISvcLocator* svcloc);
    virtual ~CscSimDataCollectionCnv();
    virtual CscSimDataCollection_PERS*   createPersistent (CscSimDataCollection* transCont);
    virtual CscSimDataCollection*        createTransient ();

private:
    CscSimDataCollectionCnv_p1    m_TPConverter_p1;
    CscSimDataCollectionCnv_p2    m_TPConverter_p2;
};

#endif
