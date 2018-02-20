// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/getDynamicAuxID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Find the auxid for a dynamic branch.
 */


#ifndef ATHCONTAINERSROOT_GETDYNAMICAUXID_H
#define ATHCONTAINERSROOT_GETDYNAMICAUXID_H


#include "AthContainersInterfaces/AuxTypes.h"
#include <typeinfo>
#include <string>



namespace SG {


/**
 * @brief Find the auxid for a dynamic branch.
 * @param ti Type of the auxiliary variable.
 *           Usually the type of the vector payload, but if @c standalone
 *           is true, then this is the type of the stored object.
 * @param name Auxiliary variable name.
 * @param elementTypeName Name of the type for one aux data element.
 *                        Should be the samr as @c branchTypeName
 *                        if @c standalone is true.
 * @param branchTypeName Name of the type for this branch.
 * @param standalone True if this is a standalone object.
 */
SG::auxid_t getDynamicAuxID (const std::type_info& ti,
                             const std::string& name,
                             const std::string& elementTypeName,
                             const std::string& branch_type_name,
                             bool standalone);


} // namespace SG


#endif // not ATHCONTAINERSROOT_GETDYNAMICAUXID_H
