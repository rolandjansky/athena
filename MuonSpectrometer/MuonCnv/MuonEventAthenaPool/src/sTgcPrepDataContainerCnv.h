/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_sTgcPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_sTgcPREPDATACONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataContainerCnv_p1.h"


// the latest persistent representation type of DataCollection:
// COMPRESS typedef  Muon::sTgcPrepDataContainer_p2  sTgcPrepDataContainer_PERS;
typedef  Muon::sTgcPrepDataContainer_p1  sTgcPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::sTgcPrepDataContainer, sTgcPrepDataContainer_PERS >  sTgcPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class sTgcPrepDataContainerCnv : 
    public sTgcPrepDataContainerCnvBase 
{
    
    friend class CnvFactory<sTgcPrepDataContainerCnv>;
    
public:
    sTgcPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~sTgcPrepDataContainerCnv();
    
    virtual sTgcPrepDataContainer_PERS*   createPersistent (Muon::sTgcPrepDataContainer* transCont);
    virtual Muon::sTgcPrepDataContainer*  createTransient ();

    virtual StatusCode initialize();
        
private:
    Muon::sTgcPrepDataContainerCnv_p1    m_converter_p1;
};

#endif
