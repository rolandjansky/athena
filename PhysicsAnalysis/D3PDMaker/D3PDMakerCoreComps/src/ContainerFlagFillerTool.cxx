/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/src/ContainerFlagFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>, from earlier code from Attila.
 * @date Apr, 2012
 * @brief Flag objects that are present in other containers.
 */


#include "ContainerFlagFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


ContainerFlagFillerTool::ContainerFlagFillerTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : BlockFillerToolImpl (type, name, parent),
    m_getters (this),
    m_map (this, m_getters, m_allowMissing)
{
  declareProperty ("FlagNames", m_flagNames,
                   "Names of the flags added to the D3PD."
"May be followed by a string of the form `@TYPE/KEY', in which case the "
"corresponding entry in Getters is automatically initialized (only available "
"when the Python configuration wrapper is used).  May be followed by a "
"docstring following a colon.");
  declareProperty ("Getters", m_getters,
                   "Getter tools for the INav4MomLinkContainer objects.");
  declareProperty ("AllowMissing", m_allowMissing,
                   "If true, missing containers will not be considered errors.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode ContainerFlagFillerTool::initialize()
{
  // Check that the configuration makes sense:
  if( m_getters.size() != m_flagNames.size() ) {
    REPORT_MESSAGE( MSG::FATAL )
      << "Different number of getter tools and flags specified";
    return StatusCode::FAILURE;
  }

  CHECK( BlockFillerToolImpl::initialize() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c fillUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode ContainerFlagFillerTool::configureD3PD (IAddVariable* tree,
                                                   const std::type_info& ti)
{
  CHECK( m_map.configureD3PD (ti) );
  CHECK( AddVariable::configureD3PD (tree) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode ContainerFlagFillerTool::book()
{
  // Resize the flag vector to the right size:
  m_flags.resize( m_flagNames.size(), 0 );

  // Book the flag variable(s):
  for( size_t i = 0; i < m_flagNames.size(); ++i ) {
    std::string flagname = m_flagNames[i];
    std::string desc = "Was this object selected according to X criteria?";
    std::string::size_type ipos = flagname.find (':');
    if (ipos != std::string::npos) {
      desc = flagname.substr (ipos+1, std::string::npos);
      flagname = flagname.substr (0, ipos);
    }
    CHECK( addVariable (flagname, m_flags[ i ], desc) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block.
 * @param p The input object.
 * @param again Set if this is a subsequent call requested by an AGAIN return
 *
 * This is called once per object.  The type of the object at which @c p
 * points is given by the @c ti argument to @c configure.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode ContainerFlagFillerTool::fillUntyped (const void* p,
                                                 bool /*again = false*/)
{
  if (m_flags.empty())
    return StatusCode(EMPTY);

  CHECK( m_map.reset() );
  for (size_t i = 0; i < m_flags.size(); i++)
    *m_flags[i] = (m_map.findIndex (p, i) != -1);
  return StatusCode::SUCCESS;
}


} // namespace D3PD
