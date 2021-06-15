/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__UNIT_TEST_SERVICE1_H
#define ASG_TOOLS__UNIT_TEST_SERVICE1_H

#include <AsgServices/AsgService.h>
#include <AsgExampleTools/IUnitTestService1.h>

namespace asg
{
  /// \brief a service used to unit test AnaToolHandle
  ///
  /// This allows to unit test the various capabilities of
  /// AnaToolHandle in a controlled fashion.

  struct UnitTestService1 : virtual public IUnitTestService1,
                            public AsgService
  {
    /// \brief standard constructor
  public:
    UnitTestService1 (const std::string& name, ISvcLocator* pSvcLocator);

    /// \brief standard destructor
  public:
    ~UnitTestService1 ();

  public:
    StatusCode initialize () override;

  public:
    virtual std::string getPropertyString () const override;

  public:
    virtual int getPropertyInt () const override;

  public:
    virtual void setPropertyInt (int val_property) override;

  public:
    virtual bool isInitialized () const override;

    /// \brief whether initialize has been called
  public:
    bool m_isInitialized = false;

    /// \brief the string property
  public:
    std::string m_propertyString;

    /// \brief the integer property
  public:
    int m_propertyInt = -7;

    /// \brief whether initialize should fail
  public:
    bool m_initializeFail = false;

    /// \brief the number of times the service of the given name has been
    /// instantiated
  public:
    static int& instance_counts (const std::string& name);
  };
}

#endif
