/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj1FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief D3PD maker regression tests.
 */


#include "Obj1FillerTool.h"
#include "D3PDMakerTest/Obj1.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj1FillerTool::Obj1FillerTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : D3PD::BlockFillerTool<Obj1> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode Obj1FillerTool::book()
{
  CHECK( addVariable ("c",   m_c) );
  CHECK( addVariable ("uc",  m_uc) );
  CHECK( addVariable ("s",   m_s) );
  CHECK( addVariable ("us",  m_us) );
  CHECK( addVariable ("i",   m_i) );
  CHECK( addVariable ("ui",  m_ui) );
  CHECK( addVariable ("f",   m_f) );
  CHECK( addVariable ("d",   m_d) );
  //CHECK( addVariable ("ll",  m_ll) );
  //CHECK( addVariable ("ull", m_ull) );
  CHECK( addVariable ("b",   m_b) );

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
StatusCode Obj1FillerTool::fill (const Obj1& p)
{
  *m_c   = p.m_c;
  *m_uc  = p.m_uc;
  *m_s   = p.m_s;
  *m_us  = p.m_us;
  *m_i   = p.m_i;
  *m_ui  = p.m_ui;
  *m_f   = p.m_f;
  *m_d   = p.m_d;
  //*m_ll  = p.m_ll;
  //*m_ull = p.m_ull;
  *m_b   = p.m_b;

  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
