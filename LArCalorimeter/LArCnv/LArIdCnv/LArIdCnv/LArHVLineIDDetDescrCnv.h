/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: LArHVLineIDDetDescrCnv.h,v 1.3 2009-02-11 04:50:25 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef LARIDCNV_LARHVLINEIDDETDESCRCNV_H
# define LARIDCNV_LARHVLINEIDDETDESCRCNV_H

//<<<<<< INCLUDES 

/////                                                      >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  This class is a converter for the LArHVLineID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class LArHVLineIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<LArHVLineIDDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long  storageType();
    static const CLID& classID();

protected:
    LArHVLineIDDetDescrCnv(ISvcLocator* svcloc);
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // LARIDCNV_LARHVLINEIDDETDESCRCNV_H
