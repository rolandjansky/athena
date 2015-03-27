/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj2FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief D3PD maker regression tests.
 */


#include "Obj2FillerTool.h"
#include "D3PDMakerTest/Obj2.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj2FillerTool::Obj2FillerTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : D3PD::BlockFillerTool<Obj2> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode Obj2FillerTool::book()
{
  CHECK( addVariable ("i",   m_i) );
  CHECK( addVariable ("f",   m_f) );

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
StatusCode Obj2FillerTool::fill (const Obj2& p)
{
  *m_i   = p.m_i;
  *m_f   = p.m_f;

  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
