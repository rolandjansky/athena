/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "VerboseSelector.h"

namespace G4UA
{

  class VerboseSelectorTool : public UserActionToolBase<VerboseSelector>
  {

  public:
    VerboseSelectorTool(const std::string& type, const std::string& name,const IInterface* parent);
  protected:
    virtual std::unique_ptr<VerboseSelector>
    makeAndFillAction(G4AtlasUserActions&) override final;
  private:
    VerboseSelector::Config m_config;
  }; // class VerboseSelectorTool

} // namespace G4UA

#endif
