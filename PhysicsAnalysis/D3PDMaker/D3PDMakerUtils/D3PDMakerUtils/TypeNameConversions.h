// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/TypeNameConversions.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Utility functions for converting between type names, type_info,
 *        and CLIDs.
 */


#ifndef D3PDMAKERUTILS_TYPENAMECONVERSIONS_H
#define D3PDMAKERUTILS_TYPENAMECONVERSIONS_H


#include "AthenaKernel/IClassIDSvc.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include <typeinfo>
#include <string>


namespace D3PD {


/**
 * @brief Convert from a class name to a type_info.
 * @param name The class name to convert.
 * @param ti[out] The resulting type_info..
 * @param context Context name, for error reporting.
 * @param clidsvc Handle to the CLID service.
 */
StatusCode nameToTypeinfo (const std::string& name,
                           std::type_info const* &ti,
                           const std::string& context,
                           ServiceHandle<IClassIDSvc> clidsvc = 
                             ServiceHandle<IClassIDSvc>("ClassIDSvc",
                                                        "TypeNameConversions"));



/**
 * @brief Convert from a class name to a CLID.
 * @param name The class name to convert.
 * @param ti[out] The resulting type_info.
 * @param context Context name, for error reporting.
 * @param clidsvc Handle to the CLID service.
 */
StatusCode nameToCLID (const std::string& name,
                       CLID& clid,
                       const std::string& context,
                       ServiceHandle<IClassIDSvc> clidsvc = 
                         ServiceHandle<IClassIDSvc>("ClassIDSvc",
                                                    "TypeNameConversions"));


/**
 * @brief Convert from a CLID to a type_info; we also already know the name.
 * @param clid The CLID to convert.
 * @param name The name of the class to convert.
 * @param ti[out] The resulting type_info.
 * @param context Context name, for error reporting.
 */
StatusCode clidToTypeinfo (CLID clid,
                           const std::string& nmae,
                           std::type_info const* &ti,
                           const std::string& context);


/**
 * @brief Convert from a type_info to a name.
 * @param ti The type_info to convert.
 */
std::string typeinfoToName (const std::type_info& ti);


} // namespace D3PD


#endif // not D3PDMAKERUTILS_TYPENAMECONVERSIONS_H
