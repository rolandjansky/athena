/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATIONTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "CheckActivation.h"

namespace G4UA
{

  /// @class CheckActivationTool
  /// @brief Tool which manages the CheckActivation
  ///
  /// @author Andrea Di Simone
  ///
  class CheckActivationTool: public UserActionToolBase<CheckActivation>
  {
    public:
      /// Standard constructor
      CheckActivationTool(const std::string& type, const std::string& name,const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<CheckActivation>
      makeAndFillAction(G4AtlasUserActions&) override final;
    }; // class CheckActivationTool

} // namespace G4UA

#endif
