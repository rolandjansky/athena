/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MDTPREPDATACONTAINERCNV_H
#define MUONEVENTATHENAPOOL_MDTPREPDATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonEventTPCnv/MdtPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataContainerCnv_p2.h"

class StoreGateSvc;
class MdtIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
}

// the latest persistent representation type of DataCollection:
typedef  Muon::MdtPrepDataContainer_p2  MdtPrepDataContainer_PERS;
typedef  T_AthenaPoolCustomCnv<Muon::MdtPrepDataContainer, MdtPrepDataContainer_PERS >  MdtPrepDataContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MdtPrepDataContainerCnv : 
    public MdtPrepDataContainerCnvBase
{
    
    friend class CnvFactory<MdtPrepDataContainerCnv>;
    
public:
    MdtPrepDataContainerCnv(ISvcLocator* svcloc);
    virtual ~MdtPrepDataContainerCnv();
    
    virtual MdtPrepDataContainer_PERS*   createPersistent (Muon::MdtPrepDataContainer* transCont);
    virtual Muon::MdtPrepDataContainer*  createTransient ();

    // Must initialize ID helpers
    virtual StatusCode initialize();
    virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_converter_tlp1; }
        
private:
    MdtPrepDataContainerCnv_tlp1        m_converter_tlp1; //!< 'Old style' top level convertor
    Muon::MdtPrepDataContainerCnv_p2    m_converter_p2; //!< New, flattened convertor
};

#endif
