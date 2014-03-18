/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj1Obj2AssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test code: associate from an Obj1 to a single Obj2.
 */


#include "Obj1Obj2AssociationTool.h"
#include "D3PDMakerTest/Obj1.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj1Obj2AssociationTool::Obj1Obj2AssociationTool (const std::string& type,
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
const Obj2* Obj1Obj2AssociationTool::get (const Obj1& p)
{
  return &p.m_o2;
}


} // namespace D3PDTest
