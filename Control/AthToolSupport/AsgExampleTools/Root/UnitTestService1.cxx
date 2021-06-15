/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgExampleTools/UnitTestService1.h>

#include <AsgMessaging/MessageCheck.h>
#include <gtest/gtest.h>
#include <map>

//
// method implementations
//

namespace asg
{
  UnitTestService1 ::
  UnitTestService1 (const std::string& name, ISvcLocator* pSvcLocator)
    : AsgService (name, pSvcLocator)
  {
    declareServiceInterface<IUnitTestService1>();

    declareProperty ("propertyInt", m_propertyInt, "the integer property");
    declareProperty ("propertyString", m_propertyString, "the string property");
    declareProperty ("initializeFail", m_initializeFail, "whether initialize should fail");

    ++ instance_counts (name);

    ANA_MSG_INFO ("create UnitTestService1 " << this);
  }



  UnitTestService1 ::
  ~UnitTestService1 ()
  {
    ANA_MSG_INFO ("destroy UnitTestService1 " << this);

    -- instance_counts (name());
  }



  StatusCode UnitTestService1 ::
  initialize ()
  {
    ANA_MSG_INFO ("initialize UnitTestService1 " << this);
    ANA_MSG_INFO ("  propertyString: " << m_propertyString);
    ANA_MSG_INFO ("  propertyInt: " << m_propertyInt);

    if (m_initializeFail)
    {
      ATH_MSG_ERROR ("service configured to fail initialize");
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



  std::string UnitTestService1 ::
  getPropertyString () const
  {
    return m_propertyString;
  }



  int UnitTestService1 ::
  getPropertyInt () const
  {
    return m_propertyInt;
  }



  void UnitTestService1 ::
  setPropertyInt (int val_property)
  {
    m_propertyInt = val_property;
  }



  bool UnitTestService1 ::
  isInitialized () const
  {
    return m_isInitialized;
  }



  int& UnitTestService1 ::
  instance_counts (const std::string& name)
  {
    static std::map<std::string,int> counts;
    auto iter = counts.find (name);
    if (iter == counts.end())
      iter = counts.insert (std::make_pair (name, 0)).first;
    assert (iter != counts.end());
    return iter->second;
  }
}
