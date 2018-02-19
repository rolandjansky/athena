/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__HYPERSPACECATCHERTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__HYPERSPACECATCHERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "HyperspaceCatcher.h"

namespace G4UA
{

  /// Tool which creates and manages the HyperspaceCatcher user action.
  ///
  class HyperspaceCatcherTool : public UserActionToolBase<HyperspaceCatcher>
  {

  public:

    HyperspaceCatcherTool(const std::string& type, const std::string& name,const IInterface* parent);

  protected:

    virtual std::unique_ptr<HyperspaceCatcher>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:

    HyperspaceCatcher::Config m_config;

  }; // class HyperspaceCatcherTool

} // namespace G4UA

#endif
