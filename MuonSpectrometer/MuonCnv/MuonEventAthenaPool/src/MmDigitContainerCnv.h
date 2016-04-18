/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MM_DIGITCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_MM_DIGITCONTAINERCNV_H

//#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonDigitContainer/MmDigitCollection.h"
#include "MuonDigitContainer/MmDigitContainer.h"

#include "MuonEventTPCnv/MuonDigitContainer/MM_DigitContainerCnv_p1.h"
// the latest persistent representation type of DataCollection:
// COMPRESS typedef  Muon::MM_DigitContainer_p2  MM_DigitContainer_PERS;
typedef  Muon::MM_DigitContainer_p1  MM_DigitContainer_PERS;
typedef  T_AthenaPoolCustomCnv<MmDigitContainer, MM_DigitContainer_PERS >  MmDigitContainerCnvBase;

/**
 ** Create derived converter to customize the saving of identifiable
 ** container
 **/
class MmDigitContainerCnv : 
    public MmDigitContainerCnvBase 
{
    
    friend class CnvFactory<MmDigitContainerCnv>;
    
public:
    MmDigitContainerCnv(ISvcLocator* svcloc);
    virtual ~MmDigitContainerCnv();
    
    virtual MM_DigitContainer_PERS*   createPersistent (MmDigitContainer* transCont);
    virtual MmDigitContainer*  createTransient ();

    virtual StatusCode initialize();
        
private:
    Muon::MM_DigitContainerCnv_p1    m_converter;
};

#endif
