/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H
#define LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "LArG4GenShowerLib/TestActionShowerLib.h"

namespace G4UA
{

  /// @class TestActionShowerLibTool
  /// @brief A tool to manage TestActionShowerLib
  ///
  /// creates one instance of TestActionShowerLib per thread
  ///
  /// @author Andrea Di Simone
  ///
  class TestActionShowerLibTool : public UserActionToolBase<TestActionShowerLib>
  {

  public:

    /// Standard tool ctor
    TestActionShowerLibTool(const std::string& type, const std::string& name,
                            const IInterface* parent);

  protected:

    /// Creates the action
    virtual std::unique_ptr<TestActionShowerLib>
    makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class TestActionShowerLibTool

} // namespace G4UA

#endif
