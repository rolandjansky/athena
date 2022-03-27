/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

#ifndef INDETMGRDETDESCRCNV_PLR_IDDETDESCRCNV_H
#define INDETMGRDETDESCRCNV_PLR_IDDETDESCRCNV_H

#include "DetDescrCnvSvc/DetDescrConverter.h"

class PLR_ID;

/**
 **  This class is a converter for the PLR_ID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class PLR_IDDetDescrCnv: public DetDescrConverter
{

public:

    virtual long int   repSvcType() const override;
    virtual StatusCode initialize() override;
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    PLR_IDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    PLR_ID*     m_plrId{};

    /// File to be read for InDet ids
    std::string m_inDetIDFileName;

    /// Tag of RDB record for InDet ids
    std::string m_inDetIdDictTag;

    /// Internal InDet id tag
    std::string m_inDetIDTag;

    /// Whether or not 
    bool        m_doChecks{};

};

#endif // INDETMGRDETDESCRCNV_PLR_IDDETDESCRCNV_H
