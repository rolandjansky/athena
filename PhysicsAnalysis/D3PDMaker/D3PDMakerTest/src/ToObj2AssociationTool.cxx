/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/ToObj2AssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Test code: Test SingleAssociationToolMulti.
 */


#include "ToObj2AssociationTool.h"
#include "D3PDMakerTest/Obj1.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ToObj2AssociationTool::ToObj2AssociationTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Obj2* ToObj2AssociationTool::get (const Obj1& p)
{
  return &p.m_o2;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const Obj2* ToObj2AssociationTool::get (const Obj2& p)
{
  return &p;
}


} // namespace D3PDTest
