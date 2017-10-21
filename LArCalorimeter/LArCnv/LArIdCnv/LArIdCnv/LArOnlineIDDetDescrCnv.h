/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: LArOnlineIDDetDescrCnv.h,v 1.5 2009-02-11 04:50:25 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef LARIDCNV_LARONLINEIDDETDESCRCNV_H
# define LARIDCNV_LARONLINEIDDETDESCRCNV_H

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
 **  This class is a converter for the LArOnlineID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class LArOnlineIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<LArOnlineIDDetDescrCnv>;

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long  storageType();
    static const CLID& classID();

protected:
    LArOnlineIDDetDescrCnv(ISvcLocator* svcloc);
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // LARIDCNV_LARONLINEIDDETDESCRCNV_H
