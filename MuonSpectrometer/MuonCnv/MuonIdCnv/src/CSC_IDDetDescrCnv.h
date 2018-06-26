/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Muon DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CSC_IDDetDescrCnv.h,v 1.4 2004/06/04 03:30:02 ketevi Exp $
//<version>	$Name:  $

#ifndef INDETMGRDETDESCRCNV_CSC_IDDETDESCRCNV_H
# define INDETMGRDETDESCRCNV_CSC_IDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the CSC_IdHelper an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class CSC_IDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<CSC_IDDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long  storageType();
    static const CLID& classID();

protected:
    CSC_IDDetDescrCnv(ISvcLocator* svcloc);

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETMGRDETDESCRCNV_CSC_IDDETDESCRCNV_H
