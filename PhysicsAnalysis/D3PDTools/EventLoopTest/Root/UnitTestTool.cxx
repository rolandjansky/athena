/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoopTest/UnitTestTool.h>

#include <gtest/gtest.h>
#include <map>

//
// method implementations
//

namespace EL
{
  UnitTestTool ::
  UnitTestTool (const std::string& val_name)
    : AsgTool (val_name)
  {
    declareProperty ("propertyInt", m_propertyInt, "the integer property");
    declareProperty ("subtool", m_subtool, "our subtool");
  }



  UnitTestTool ::
  ~UnitTestTool ()
  {
  }



  StatusCode UnitTestTool ::
  initialize ()
  {
    return StatusCode::SUCCESS;
  }



  int UnitTestTool ::
  getPropertyInt () const
  {
    return m_propertyInt;
  }



  const IUnitTestTool *UnitTestTool ::
  getSubtool () const
  {
    if (!m_subtool.empty())
      return &*m_subtool;
    else
      return nullptr;
  }
}
