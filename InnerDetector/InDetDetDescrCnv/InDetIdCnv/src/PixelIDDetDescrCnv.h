/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: PixelIDDetDescrCnv.h,v 1.3 2007-01-01 10:47:17 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETMGRDETDESCRCNV_PIXELIDDETDESCRCNV_H
# define INDETMGRDETDESCRCNV_PIXELIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class PixelID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  This class is a converter for the PixelID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class PixelIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<PixelIDDetDescrCnv>;

public:

    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

protected:
    PixelIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    PixelID*      m_pixelId;

    /// File to be read for InDet ids
    std::string   m_inDetIDFileName;

    /// Tag of RDB record for InDet ids
    std::string   m_inDetIdDictTag;

    /// Internal InDet id tag
    std::string   m_inDetIDTag;

    /// Whether or not 
    bool          m_doChecks;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETMGRDETDESCRCNV_PIXELIDDETDESCRCNV_H
