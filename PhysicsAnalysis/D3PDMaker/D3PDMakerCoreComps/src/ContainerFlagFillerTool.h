// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerCoreComps/src/ContainerFlagFillerTool.h
 * @author scott snyder <snyder@bnl.gov>, from earlier code from Attila.
 * @date Apr, 2012
 * @brief Flag objects that are present in other containers.
 */


#ifndef D3PDMAKERCORECOMPS_CONTAINERFLAGFILLERTOOL_H
#define D3PDMAKERCORECOMPS_CONTAINERFLAGFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#include "D3PDMakerUtils/IndexMap.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Flag objects that are present in other containers.
 *
 * Given a list of variable name / collection getter pairs,
 * this sets each variable to a flag depending on whether or not
 * the input object is found in the corresponding collection getter.
 *
 * This can be used, for example, with the INav4MomLinkContainer objects
 * created by the DPD making tools, but it should work with any Collection
 * wrapped with a ICollectionGetterToool.
 */
class ContainerFlagFillerTool
  : public BlockFillerToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ContainerFlagFillerTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /// Function initializing the tool
  virtual StatusCode initialize();


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti);


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book();


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
  virtual StatusCode fillUntyped (const void* p,
                                  bool again = false);


private:
  /// Property: The flag names.
  std::vector<std::string> m_flagNames;

  /// Property: The container getters that are used to compute the flags
  ToolHandleArray<ICollectionGetterTool> m_getters;

  /// Property: If true, missing containers will not be considered errors.
  bool m_allowMissing;

  /// Variables: The flags created in the D3PD
  std::vector<bool*> m_flags;

  /// Map to manage index lookups.
  IndexMap m_map;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_CONTAINERFLAGFILLERTOOL_H
