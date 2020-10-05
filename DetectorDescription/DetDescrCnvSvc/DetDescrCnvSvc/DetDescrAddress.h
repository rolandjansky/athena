/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	DetDescrAddress.h
//--------------------------------------------------------------------
//
//	Package    : DetDescrCnvSvc 
//
//  Description:     Definition of detector description address class
//                   This class for the moment has just the
//                   funtionality of a GenericAddress. It provides the
//                   conversion service type but nothing more. It thus
//                   just holds the clid and the class name. It is
//                   created for later expansion.
//	Author     : RD Schaffer
//====================================================================
#ifndef DETDESCRCNVSVC_DETDESCRADDRESS_H
#define DETDESCRCNVSVC_DETDESCRADDRESS_H

// C/C++ include files
#include <string>

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"

const extern long DetDescr_StorageType;

class DetDescrAddress   : public GenericAddress   {
public:


    /// Standard Destructor
    virtual ~DetDescrAddress() {}; //doesn't own event

    /// Standard Constructor
    DetDescrAddress(const CLID& clid, const std::string& fname,
		    const std::string& cname );

    DetDescrAddress(const CLID& clid ) ;

    const std::string& payload() const;

    /// write technology-specific part of address to string  
    StatusCode  toString(std::string& refString) const;

    /// initialize technology-specific part of IOA from stringifiedIOA
    StatusCode  fromString(const std::string& stringifiedIOA);


private:

    std::string  m_payload;

};
#endif 





