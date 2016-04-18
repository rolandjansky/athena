/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_CSCPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_CSCPREPDATACONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

#include "MuonEventTPCnv/CscPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataContainerCnv_p2.h"


// the latest persistent representation type of DataCollection:
// COMPRESS typedef  Muon::CscPrepDataContainer_p2  CscPrepDataContainer_PERS;
typedef  Muon::CscPrepDataContainer_tlp1  CscPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::CscPrepDataContainer, CscPrepDataContainer_PERS >  CscPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class CscPrepDataContainerCnv : 
    public CscPrepDataContainerCnvBase 
{
    
    friend class CnvFactory<CscPrepDataContainerCnv>;
    
public:
    CscPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~CscPrepDataContainerCnv();
    
    virtual CscPrepDataContainer_PERS*   createPersistent (Muon::CscPrepDataContainer* transCont);
    virtual Muon::CscPrepDataContainer*  createTransient ();

    virtual StatusCode initialize();
    virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; }
        
private:
    Muon::CscPrepDataContainerCnv_p2    m_converter_p2;
    CscPrepDataContainerCnv_tlp1        m_TPConverter;
};

#endif
