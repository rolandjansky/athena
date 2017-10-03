/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SiliconIDDetDescrCnv.h,v 1.3 2007-01-01 10:47:18 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETMGRDETDESCRCNV_SILICONIDDETDESCRCNV_H
# define INDETMGRDETDESCRCNV_SILICONIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class SiliconID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  This class is a converter for the SiliconID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class SiliconIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<SiliconIDDetDescrCnv>;

public:

    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

protected:
    SiliconIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    SiliconID*       m_siliconId;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETMGRDETDESCRCNV_SILICONIDDETDESCRCNV_H
