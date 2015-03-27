/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj12FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief D3PD maker regression tests.
 */


#include "Obj12FillerTool.h"
#include "D3PDMakerTest/Obj1.h"
#include "D3PDMakerTest/Obj2.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj12FillerTool::Obj12FillerTool (const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent)
  : Base (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode Obj12FillerTool::book()
{
  CHECK( addVariable ("mult_i",   m_mult_i) );

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
StatusCode Obj12FillerTool::fill (const Obj1& p)
{
  *m_mult_i   = p.m_i;

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
StatusCode Obj12FillerTool::fill (const Obj2& p)
{
  *m_mult_i   = p.m_i;

  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
