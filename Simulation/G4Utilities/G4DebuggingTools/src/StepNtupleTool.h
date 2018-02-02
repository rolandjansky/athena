/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H
#define G4DEBUGGINGTOOLS_G4UA__STEPNTUPLETOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "StepNtuple.h"

namespace G4UA
{

  /// @class StepNtupleTool
  /// @brief Tool which manages the StepNtuple action.
  ///
  /// Create the StepNtuple for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class StepNtupleTool : public UserActionToolBase<StepNtuple>
  {

  public:
    /// standard tool ctor
    StepNtupleTool(const std::string& type, const std::string& name,
                   const IInterface* parent);
  protected:
    /// Create action for this thread
    virtual std::unique_ptr<StepNtuple>
    makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class StepNtupleTool

} // namespace G4UA

#endif
