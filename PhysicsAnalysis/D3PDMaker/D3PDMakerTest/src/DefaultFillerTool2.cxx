/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/DefaultFillerTool2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Test setting default values in d3pds.
 */


#include "DefaultFillerTool2.h"
#include "D3PDMakerTest/Obj1.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
DefaultFillerTool2::DefaultFillerTool2 (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : D3PD::BlockFillerTool<Obj1> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Book variables for this block.
 */
StatusCode DefaultFillerTool2::book()
{
  CHECK( addVariable ("c1",   m_c1) );
  CHECK( addVariable ("c2",   m_c2, "", 1) );
  CHECK( addVariable ("uc1",  m_uc1) );
  CHECK( addVariable ("uc2",  m_uc2, "", 2) );
  CHECK( addVariable ("s1",   m_s1) );
  CHECK( addVariable ("s2",   m_s2, "", -3) );
  CHECK( addVariable ("us1",  m_us1) );
  CHECK( addVariable ("us2",  m_us2, "", 4) );
  CHECK( addVariable ("i1",   m_i1) );
  CHECK( addVariable ("i2",   m_i2, "", -5) );
  CHECK( addVariable ("ui1",  m_ui1) );
  CHECK( addVariable ("ui2",  m_ui2, "", 6) );
  CHECK( addVariable ("f1",   m_f1) );
  CHECK( addVariable ("f2",   m_f2, "", -7.5) );
  CHECK( addVariable ("d1",   m_d1) );
  CHECK( addVariable ("d2",   m_d2, "", 8.5) );
  CHECK( addVariable ("b1",   m_b1) );
  CHECK( addVariable ("b2",   m_b2, "", true) );

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
StatusCode DefaultFillerTool2::fill (const Obj1& /*p*/)
{
  return StatusCode::SUCCESS;
}


} // namespace D3PDTest
