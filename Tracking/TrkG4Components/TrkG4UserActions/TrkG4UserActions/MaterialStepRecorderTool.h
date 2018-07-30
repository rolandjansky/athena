/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H
#define TRKG4USERACTIONS_G4UA__MATERIALSTEPRECORDERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "TrkG4UserActions/MaterialStepRecorder.h"

namespace G4UA
{

  /// @class MaterialStepRecorderTool
  /// @brief Tool which manages the MaterialStepRecorder action
  ///
  /// @author Andrea Di Simone
  ///
  class MaterialStepRecorderTool : public UserActionToolBase<MaterialStepRecorder>
  {

  public:
    /// Standard constructor
    MaterialStepRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<MaterialStepRecorder>
    makeAndFillAction(G4AtlasUserActions&) override final;
  }; // class MaterialStepRecorderTool

} // namespace G4UA

#endif
