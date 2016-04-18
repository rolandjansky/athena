/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MMPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_MMPREPDATACONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataContainerCnv_p1.h"


// the latest persistent representation type of DataCollection:
// COMPRESS typedef  Muon::MMPrepDataContainer_p2  MMPrepDataContainer_PERS;
typedef  Muon::MMPrepDataContainer_p1  MMPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::MMPrepDataContainer, MMPrepDataContainer_PERS >  MMPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MMPrepDataContainerCnv : 
    public MMPrepDataContainerCnvBase 
{
    
    friend class CnvFactory<MMPrepDataContainerCnv>;
    
public:
    MMPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~MMPrepDataContainerCnv();
    
    virtual MMPrepDataContainer_PERS*   createPersistent (Muon::MMPrepDataContainer* transCont);
    virtual Muon::MMPrepDataContainer*  createTransient ();

    virtual StatusCode initialize();
        
private:
    Muon::MMPrepDataContainerCnv_p1    m_converter_p1;
};

#endif
