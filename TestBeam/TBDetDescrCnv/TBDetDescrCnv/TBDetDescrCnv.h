/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Package for Testbeam Detector Description converter
 -------------------------------------------------
 ***************************************************************************/

#ifndef TBDETDESCRCNV_TBDETDESCRCNV_H
#define TBDETDESCRCNV_TBDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


class TBDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<TBDetDescrCnv>;

public:

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    virtual long repSvcType() const;
    static long storageType();
    static const CLID& classID();

protected:

    TBDetDescrCnv(ISvcLocator* svcloc);

private:

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // TBDETDESCRCNV_TBDETDESCRCNV_H

