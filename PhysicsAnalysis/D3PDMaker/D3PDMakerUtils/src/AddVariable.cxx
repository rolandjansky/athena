/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/src/AddVariable.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Common implementation of @c IAddVariable.
 */


#include "AthenaKernel/errorcheck.h"
#include "D3PDMakerUtils/AddVariable.h"


namespace D3PD {


/// Used to provide a default value for @a blockName.
std::string AddVariable::s_emptyString;


/**
 * @brief Constructor.
 * @param prefix Reference to the variable prefix.
 * @param blockName Reference to the block name for this tool.
 */
AddVariable::AddVariable (const std::string& prefix,
                          const std::string& /*blockName*/ /*= s_emptyString*/)
  : m_metadata (),
    m_prefix (prefix),
    m_suffix (s_emptyString),
    //m_blockName (blockName),
    m_tree (0)
{
}


/**
 * @brief Constructor.
 * @param prefix Reference to the variable prefix.
 * @param blockName Reference to the block name for this tool.
 */
AddVariable::AddVariable (const std::string& prefix,
                          const std::string& /*blockName*/,
                          const std::string& suffix)
  : m_metadata (),
    m_prefix (prefix),
    m_suffix (suffix),
    //m_blockName (blockName),
    m_tree (0)
{
}


/**
 * @brief Configure the parent tree.
 * @param tree The parent @c IAddVariable instance.
 */
StatusCode AddVariable::configureD3PD (IAddVariable* tree)
{
  m_tree = tree;
  m_metadata.clear();
  return StatusCode::SUCCESS;
}


/**
 * @brief Add a variable to the tuple.
 * @param name The name of the variable.
 * @param type The type of the variable.
 * @param ptr Pointer to the type of the variable.
 *            The pointer need not be initialized;
 *            the D3PD software will set the pointer
 *            prior to calling @c fill().
 * @param docstring Documentation string for this variable.
 * @param defval Pointer to the default value to use for this variable.
 *               Null for no default (generally means to fill with zeros).
 *               Of the type given by @c ti.
 *               Only works for basic types.
 *
 * If called from the constructor, the only effect is to clear @c ptr.
 */
StatusCode AddVariable::addVariable (const std::string& name,
                                     const std::type_info& ti,
                                     void* & ptr,
                                     const std::string& docstring /*= ""*/,
                                     const void* defval /*= 0*/)
{
  if (!m_tree) {
    // Called from constructor --- just initialize pointer.
    ptr = 0;
    return StatusCode::SUCCESS;
  }

  CHECK( m_metadata.addVariable( m_prefix + name + m_suffix,
                                 ti, ptr, docstring, defval ) );
  return m_tree->addVariable (m_prefix + name + m_suffix,
                              ti, ptr, docstring, defval);
}


/**
 * @brief Add a variable to the tuple.
 * @param name The name of the variable.
 * @param type The type of the variable.
 * @param ptr Pointer to the type of the variable.
 *            The pointer need not be initialized;
 *            the D3PD software will set the pointer
 *            prior to calling @c fill().
 * @param dim Dimension for the variable.
 *            (Presently unimplemented!)
 * @param docstring Documentation string for this variable.
 * @param defval Pointer to the default value to use for this variable.
 *               Null for no default (generally means to fill with zeros).
 *               Of the type given by @c ti.
 *               Only works for basic types.
 *
 * If called from the constructor, the only effect is to clear @c ptr.
 */
StatusCode
AddVariable::addDimensionedVariable (const std::string& name,
                                     const std::type_info& ti,
                                     void* & ptr,
                                     const std::string& dim,
                                     const std::string& docstring /*= ""*/,
                                     const void* defval /*= 0*/)
{
  if (!m_tree) {
    // Called from constructor --- just initialize pointer.
    ptr = 0;
    return StatusCode::SUCCESS;
  }

  CHECK( m_metadata.addDimensionedVariable( m_prefix + name + m_suffix,
                                            ti, ptr, dim,
                                            docstring, defval ) );
  return m_tree->addDimensionedVariable (m_prefix + name + m_suffix,
                                         ti, ptr, dim,
                                         docstring, defval);
}


} // namespace D3PD
