/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/IdentityAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief A generic identity association; i.e., one that simply returns
 *        its input.
 */

#include "AthenaKernel/errorcheck.h"

#include "IdentityAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
IdentityAssociationTool::IdentityAssociationTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : SingleAssociationToolImpl (type, name, parent),
    m_ti(0)
{
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c getUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * (to @c getUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode IdentityAssociationTool::configureD3PD (D3PD::IAddVariable* tree,
                                                   const std::type_info& ti)
{
  CHECK( AddVariable::configureD3PD (tree) );
  m_ti = &ti;
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the @c std::type_info for the source of the association.
 */
const std::type_info& IdentityAssociationTool::fromTypeinfo() const
{
  return *m_ti;
}


/**
 * @brief Return the type of object retrieved by this tool.
 */
const std::type_info& IdentityAssociationTool::typeinfo() const
{
  return *m_ti;
}
  

/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 * Should be of the type given by @c typeinfo.
 */
const void* IdentityAssociationTool::getUntyped (const void* p)
{
  return p;
}


} // namespace D3PD
