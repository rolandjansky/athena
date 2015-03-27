/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj5FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief D3PD maker regression tests.
 */


#include "Obj5FillerTool.h"
#include "D3PDMakerTest/Obj5.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj5FillerTool::Obj5FillerTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : D3PD::BlockFillerTool<Obj5> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode Obj5FillerTool::book()
{
  CHECK( addVariable ("obj5",   m_obj5) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode Obj5FillerTool::fill (const Obj5& p)
{
  *m_obj5 = p.m_obj5;
  
  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
