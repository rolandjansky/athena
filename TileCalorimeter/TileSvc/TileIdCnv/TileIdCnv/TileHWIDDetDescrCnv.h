/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileHWIDDetDescrCnv.h,v 1.5 2009-02-12 10:11:08 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef TILEIDCNV_TILEHWIDDETDESCRCNV_H
# define TILEIDCNV_TILEHWIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  This class is a converter for the TileHWID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class TileHWIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<TileHWIDDetDescrCnv>;

public:
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    virtual long repSvcType() const;
    static long storageType();
    static const CLID& classID();

protected:
    TileHWIDDetDescrCnv(ISvcLocator* svcloc);
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // TILEDETMGRDETDESCRCNV_TILEHWIDDETDESCRCNV_H

