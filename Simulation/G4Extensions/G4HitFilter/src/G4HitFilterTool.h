/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_G4UA__G4HITFILTERTOOL_H
#define G4HITFILTER_G4UA__G4HITFILTERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "G4HitFilter.h"


namespace G4UA
{

  /// @class G4HitFilterTool
  /// Tool which constructs/manages the G4HitFilter user action.
  ///
  class G4HitFilterTool : public UserActionToolBase<G4HitFilter>
  {

    public:

      /// Standard constructor
      G4HitFilterTool(const std::string& type, const std::string& name,
                      const IInterface* parent);

      /// Finalize the tool
      virtual StatusCode finalize() override;

    protected:

      /// Construct the user action for current worker thread
      virtual std::unique_ptr<G4HitFilter>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// User action configuration options
      G4HitFilter::Config m_config;

  }; // class G4HitFilterTool

} // namespace G4UA

#endif
