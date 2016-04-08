// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/AddVariable.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Common implementation of @c IAddVariable.
 */

#ifndef D3PDMAKERUTILS_ADDVARIABLE_H
#define D3PDMAKERUTILS_ADDVARIABLE_H


#include "D3PDMakerInterfaces/IAddVariable.h"
#include "ObjectMetadata.h"


namespace D3PD {


/**
 * @brief Common implementation of @c IAddVariable.
 *
 * This implements the @c IAddVariable interface for the common case
 * where we just need to add a prefix/suffix to the variable name and forward
 * to another @c IAddVariable.
 *
 * References to the prefix/suffix and block name are passed to the constructor
 * (these will typically be tool properties) and the parent @c IAddVariable
 * is passed to @c configureD3PD.
 */
class AddVariable
  : public IAddVariable
{
private:
  /// Used to provide a default value for @a blockName.
  static std::string s_emptyString;


public:
  /**
   * @brief Constructor.
   * @param prefix Reference to the variable prefix.
   * @param blockName Reference to the block name for this tool.
   */
  AddVariable (const std::string& prefix,
               const std::string& blockName = s_emptyString);


  /**
   * @brief Constructor.
   * @param prefix Reference to the variable prefix.
   * @param blockName Reference to the block name for this tool.
   * @param suffix Reference to the variable suffix.
   */
  AddVariable (const std::string& prefix,
               const std::string& blockName,
               const std::string& suffix);


  /**
   * @brief Configure the parent tree.
   * @param tree The parent @c IAddVariable instance.
   */
  StatusCode configureD3PD (IAddVariable* tree);


  /// Make the template implementation from @c IAddVariable visible.
  using IAddVariable::addVariable;
  using IAddVariable::addDimensionedVariable;


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
  virtual StatusCode addVariable (const std::string& name,
                                  const std::type_info& ti,
                                  void* & ptr,
                                  const std::string& docstring = "",
                                  const void* defval = 0);


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
  virtual StatusCode
  addDimensionedVariable (const std::string& name,
                          const std::type_info& ti,
                          void* & ptr,
                          const std::string& dim,
                          const std::string& docstring = "",
                          const void* defval = 0);


protected:
  /// Metadata about the variables created by this object
  ObjectMetadata m_metadata;


private:
  /// Reference to the variable prefix.
  const std::string& m_prefix;

  /// Reference to the variable suffix.
  const std::string& m_suffix;

  /// Reference to the block name.
  //const std::string& m_blockName;

  /// The parent @c IAddVariable instance.
  IAddVariable* m_tree;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_ADDVARIABLE_H
