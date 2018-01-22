/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__G4ATLANTISDUMPERTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__G4ATLANTISDUMPERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "G4AtlantisDumper.h"

namespace G4UA
{

  class G4AtlantisDumperTool : public UserActionToolBase<G4AtlantisDumper>
  {

  public:

    G4AtlantisDumperTool(const std::string& type, const std::string& name, const IInterface* parent);

  protected:

    virtual std::unique_ptr<G4AtlantisDumper>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:

    G4AtlantisDumper::Config m_config;

  }; // class G4AtlantisDumperTool

} // namespace G4UA

#endif
