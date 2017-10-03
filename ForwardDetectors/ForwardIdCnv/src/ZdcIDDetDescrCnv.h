/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ForwardIdCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id:  $
//<version>	$Name: not supported by cvs2svn $

#ifndef FORWARDIDCNV_ZDCIDDETDESCRCNV_H
# define FORWARDIDCNV_ZDCIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class ZdcID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  This class is a converter for the ZdcID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class ZdcIDDetDescrCnv: public DetDescrConverter {
    friend class CnvFactory<ZdcIDDetDescrCnv>;

public:

    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

protected:
    ZdcIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    ZdcID*      m_zdcId;

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

#endif // FORWARDIDCNV_ZDCIDDETDESCRCNV_H
