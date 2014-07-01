// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DummyVarFillerTool.h 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/DummyVarFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Create an empty variable in the D3PD.
 */


#ifndef D3PDMAKERCORECOMPS_DUMMYVARFILLERTOOL_H
#define D3PDMAKERCORECOMPS_DUMMYVARFILLERTOOL_H


#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include <string>


namespace D3PD {


/**
 * @brief Create an empty variable in the D3PD.
 *
 * This can be used to store metadata about the structure of the tree.
 */
class DummyVarFillerTool
  : public BlockFillerTool<void>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  DummyVarFillerTool (const std::string& type,
                      const std::string& name,
                      const IInterface* parent);


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill ();


private:
  /// Property: Name of the variable to create.
  std::string m_varName;

  /// Variable: The created variable.
  char* m_var;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_DUMMYVARFILLERTOOL_H
