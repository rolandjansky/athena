/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETMGRDETDESCRCNV_CALODETMGRDETDESCRCNV_H
#define CALODETMGRDETDESCRCNV_CALODETMGRDETDESCRCNV_H

#include "DetDescrCnvSvc/DetDescrConverter.h"

class CaloMgrDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    CaloMgrDetDescrCnv(ISvcLocator* svcloc);

private:

};

#endif
