/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj4FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2011
 * @brief D3PD maker regression tests.
 */


#include "Obj4FillerTool.h"
#include "D3PDMakerTest/Obj4.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj4FillerTool::Obj4FillerTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : D3PD::BlockFillerTool<Obj4> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode Obj4FillerTool::book()
{
  CHECK( addVariable ("obj4",   m_obj4) );

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
StatusCode Obj4FillerTool::fill (const Obj4& p)
{
  *m_obj4 = p.m_obj4;
  
  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
