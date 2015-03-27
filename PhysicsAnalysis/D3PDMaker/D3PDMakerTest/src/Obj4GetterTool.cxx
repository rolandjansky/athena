/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj4GetterTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2011
 * @brief Test getter tool for Obj4.
 */


#include "Obj4GetterTool.h"


namespace D3PDTest {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
Obj4GetterTool::Obj4GetterTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : D3PD::SGCollectionGetterTool<Obj4Container> (type, name, parent)
{
}


} // namespace D3PDTest
