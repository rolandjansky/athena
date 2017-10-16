/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MUONSIMDATACOLLECTIONCNV_H
#define MUONEVENTATHENAPOOL_MUONSIMDATACOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollectionCnv_p2.h"

// the latest persistent representation type of DataCollection:
typedef  Muon::MuonSimDataCollection_p1  MuonSimDataCollection_PERS;
typedef  T_AthenaPoolCustomCnv<MuonSimDataCollection, MuonSimDataCollection_PERS >  MuonSimDataCollectionCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MuonSimDataCollectionCnv : public MuonSimDataCollectionCnvBase
{

    friend class CnvFactory<MuonSimDataCollectionCnv>;

public:
    MuonSimDataCollectionCnv(ISvcLocator* svcloc);
    virtual ~MuonSimDataCollectionCnv();
    virtual MuonSimDataCollection_PERS*   createPersistent (MuonSimDataCollection* transCont);
    virtual MuonSimDataCollection*        createTransient ();

private:
    MuonSimDataCollectionCnv_p1    m_TPConverter_p1;
    MuonSimDataCollectionCnv_p2    m_TPConverter_p2;
};

#endif
