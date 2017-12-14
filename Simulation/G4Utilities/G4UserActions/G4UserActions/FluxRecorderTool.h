/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H
#define G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H

#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"
#include "G4UserActions/FluxRecorder.h"

namespace G4UA
{

  /// @class FluxRecorderTool
  /// @brief Tool which manages the FluxRecorder action.
  ///
  /// Create the FluxRecorder  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class FluxRecorderTool : public UserActionToolBase<FluxRecorder>
  {

    public:

      /// Standard tool ctor
      FluxRecorderTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

    protected:

      /// create action for this thread
      virtual std::unique_ptr<FluxRecorder>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class FluxRecorderTool

} // namespace G4UA
#endif
