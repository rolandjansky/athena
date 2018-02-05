/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHADRONS_G4UA__SG_STEPNTUPLETOOL_H
#define RHADRONS_G4UA__SG_STEPNTUPLETOOL_H

#include "G4AtlasTools/UserActionToolBase.h"

#include "SG_StepNtuple.h"

namespace G4UA
{

  /// @class SG_StepNtupleTool
  /// @brief Tool which manages the SG_StepNtuple user action.
  ///  creates one SG_StepNtuple instance per thread
  /// @author Andrea Di Simone
  ///
  class SG_StepNtupleTool : public UserActionToolBase<SG_StepNtuple>
  {

  public:

    /// standard tool ctor
    SG_StepNtupleTool(const std::string& type, const std::string& name,
                      const IInterface* parent);
  protected:

    /// creates the action instances
    virtual std::unique_ptr<SG_StepNtuple>
    makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class SG_StepNtupleTool

} // namespace G4UA

#endif
