/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_STGC_DIGITCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_STGC_DIGITCONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonDigitContainer/sTgcDigitCollection.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitContainerCnv_p1.h"
// the latest persistent representation type of DataCollection:
// COMPRESS typedef  Muon::STGC_DigitContainer_p2  STGC_DigitContainer_PERS;
typedef  Muon::STGC_DigitContainer_p1  STGC_DigitContainer_PERS;
typedef  T_AthenaPoolCustomCnv<sTgcDigitContainer, STGC_DigitContainer_PERS >  sTgcDigitContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class sTgcDigitContainerCnv : 
    public sTgcDigitContainerCnvBase 
{
    
    friend class CnvFactory<sTgcDigitContainerCnv>;
    
public:
    sTgcDigitContainerCnv(ISvcLocator* svcloc);
    virtual ~sTgcDigitContainerCnv();
    
    virtual STGC_DigitContainer_PERS*   createPersistent (sTgcDigitContainer* transCont);
    virtual sTgcDigitContainer*           createTransient ();

    virtual StatusCode initialize();
        
private:
    Muon::STGC_DigitContainerCnv_p1    m_converter;
};

#endif
