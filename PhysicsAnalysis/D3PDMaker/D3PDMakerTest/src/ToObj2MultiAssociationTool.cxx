/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/ToObj2MultiAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test code: Test MultiAssociationToolMulti.
 */


#include "ToObj2MultiAssociationTool.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ToObj2MultiAssociationTool::ToObj2MultiAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("Which", m_which = 0);
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode ToObj2MultiAssociationTool::reset (const Obj1& p)
{
  if (m_which == 0) {
    m_it = p.m_vo2.begin();
    m_end = p.m_vo2.end();
  }
  else {
    m_it = p.m_vo3.begin();
    m_end = p.m_vo3.end();
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode ToObj2MultiAssociationTool::reset (const Obj2& p)
{
  m_tmpvec.clear();
  m_tmpvec.push_back (p);
  m_it = m_tmpvec.begin();
  m_end = m_tmpvec.end();
  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Obj2* ToObj2MultiAssociationTool::next ()
{
  if (m_it == m_end)
    return 0;
  const Obj2* p = &*m_it;
  ++m_it;
  return p;
}


} // namespace D3PDTest
