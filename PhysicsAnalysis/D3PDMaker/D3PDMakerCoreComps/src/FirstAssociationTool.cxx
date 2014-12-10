/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/FirstAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Choose the first result from a multiple association.
 */


#include "FirstAssociationTool.h"
#include "D3PDMakerInterfaces/IMultiAssociationTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
FirstAssociationTool::FirstAssociationTool (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : base_class (type, name, parent),
    m_associator (this)
{
  declareProperty ("Associator", m_associator,
                   "The IMultiAssociationTool instance to wrap.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode FirstAssociationTool::initialize()
{
  CHECK( AthAlgTool::initialize() );
  CHECK( m_associator.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c resetUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode FirstAssociationTool::configureD3PD (IAddVariable* tree,
                                                const std::type_info& ti)
{
  CHECK( m_associator->configureD3PD (tree, ti) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode FirstAssociationTool::book()
{
  CHECK( m_associator->book() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the type of object retrieved by this tool.
 */
const std::type_info& FirstAssociationTool::typeinfo() const
{
  return m_associator->elementTypeinfo();

}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 * Should be of the type given by @c typeinfo.
 */
const void* FirstAssociationTool::getUntyped (const void* p)
{
  StatusCode sc = m_associator->resetUntyped (p);
  if (sc.isFailure())
    return 0;
  return m_associator->nextUntyped();
}


/**
 * @brief Release an object retrieved from the association.
 * @param p The object to release.
 *
 * Call this when you are done with the object returned by
 * @c getUntyped().  The default implementation is a no-op,
 * but if the association dynamically allocated the object which
 * it returned, this gives it a chance to free it.
 */
void FirstAssociationTool::releaseObjectUntyped (const void* p)
{
  m_associator->releaseElementUntyped (p);
}


} // namespace D3PD
