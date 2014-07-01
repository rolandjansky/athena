/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DummyVarFillerTool.cxx 491462 2012-03-28 23:35:19Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/DummyVarFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Create an empty variable in the D3PD.
 */


#include "DummyVarFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
DummyVarFillerTool::DummyVarFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent)
{
  declareProperty ("VarName", m_varName, "Name of the variable to create.");
  book().ignore(); // Clear variables.
}


/**
 * @brief Declare tuple variables.
 *
 * This is called at the start of the first event.
 */
StatusCode DummyVarFillerTool::book()
{
  CHECK( addVariable (m_varName, m_var, "(Dummy variable.)") );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode DummyVarFillerTool::fill ()
{
  return StatusCode::SUCCESS;
}


} // namespace D3PD
