/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H
#define G4ATLASTESTS_G4UA__STEPPINGVALIDATIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "SteppingValidation.h"

namespace G4UA
{

  /// @class SteppingValidationTool
  /// @brief a tool to manage the SteppingValidation action in AthenaMT
  ///
  /// creates one instance of the action per thread
  ///
  /// @author Andrea Di Simone
  ///
  class SteppingValidationTool : public UserActionToolBase<SteppingValidation>
  {

  public:

    /// standard tool ctor
    SteppingValidationTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

  protected:

    /// creates the instance of the action for this thread
    virtual std::unique_ptr<SteppingValidation>
    makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class SteppingValidationTool

} // namespace G4UA

#endif
