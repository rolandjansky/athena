/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgExampleTools/UnitTestTool1A.h>

//
// method implementations
//

namespace asg
{
  UnitTestTool1A ::
  UnitTestTool1A (const std::string& val_name)
    : AsgTool (val_name)
  {
    declareProperty ("propertyInt", m_propertyInt, "the integer property");
    declareProperty ("propertyString", m_propertyString, "the string property");
    declareProperty ("initializeFail", m_initializeFail, "whether initialize should fail");
  }



  StatusCode UnitTestTool1A ::
  initialize ()
  {
    if (m_initializeFail)
    {
      ATH_MSG_ERROR ("tool configured to fail initialize");
      return StatusCode::FAILURE;
    }
    if (m_isInitialized)
    {
      ATH_MSG_ERROR ("initialize called twice");
      return StatusCode::FAILURE;
    }
    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  std::string UnitTestTool1A ::
  getPropertyString () const
  {
    return m_propertyString;
  }



  int UnitTestTool1A ::
  getPropertyInt () const
  {
    return m_propertyInt;
  }



  void UnitTestTool1A ::
  setPropertyInt (int val_property)
  {
    m_propertyInt = val_property;
  }



  bool UnitTestTool1A ::
  isInitialized () const
  {
    return m_isInitialized;
  }
}
