/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H
#define G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "FluxRecorder.h"

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
