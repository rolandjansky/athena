/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__I_UNIT_TEST_SERVICE1_H
#define ASG_TOOLS__I_UNIT_TEST_SERVICE1_H

#include <AsgServices/IAsgService.h>

namespace asg
{
  /// \brief the interface for \ref UnitTestService1

  class IUnitTestService1 : virtual public IAsgService
  {
    /// declare the interface that this class provides
  public:
    DeclareInterfaceID( asg::IUnitTestService1, 1, 0 );

    /// \brief get the integer property
  public:
    virtual std::string getPropertyString () const = 0;

    /// \brief get the integer property
  public:
    virtual int getPropertyInt () const = 0;

    /// \brief get the integer property
  public:
    virtual void setPropertyInt (int val_property) = 0;

    /// \brief get whether we have been initialized
  public:
    virtual bool isInitialized () const = 0;
  };
}

#endif
