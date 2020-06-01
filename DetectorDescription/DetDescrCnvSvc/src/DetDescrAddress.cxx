/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	DetDescrAddress implementation
//--------------------------------------------------------------------
//
//	Package    : DetDescrCnvSvc
//
//  Description:     Detector description address implementation
//
//	Author     : RD Schaffer
//      created    : June 2002
//  History    :
//====================================================================

//own 
#include "DetDescrCnvSvc/DetDescrAddress.h"

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"


/// Standard Constructor
DetDescrAddress::DetDescrAddress(const CLID& clid, 
const std::string& fname , const std::string& cname)
 : GenericAddress(DetDescr_StorageType, clid,fname,cname)
{
}

DetDescrAddress::DetDescrAddress(const CLID& clid) 
  : GenericAddress(DetDescr_StorageType, clid,"","")
{

}

const std::string& 
DetDescrAddress::payload() const
{
    return m_payload;
}

StatusCode 
DetDescrAddress::toString(std::string& refString) const
{
    refString += m_payload;
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode
DetDescrAddress::fromString(const std::string& stringifiedIOA)
{
    m_payload = stringifiedIOA;
    return StatusCode::SUCCESS;
}

