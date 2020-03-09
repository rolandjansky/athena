/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGPLANETOOL_H
#define G4USERACTIONS_G4UA__SCORINGPLANETOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "ScoringPlane.h"


namespace G4UA
{

  /// @class ScoringPlaneTool
  /// @brief A tool to manage ScoprinPlane actions
  ///
  /// creates one ScoprinPlane instance per thread
  /// @author Andrea Di Simone
  ///
  class ScoringPlaneTool : public UserActionToolBase<ScoringPlane>
  {

    public:

      /// Standard tool ctor
      ScoringPlaneTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

    protected:

      /// creates the action instances
      virtual std::unique_ptr<ScoringPlane>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// holds the python configuration
      ScoringPlane::Config m_config;

  }; // class ScoringPlaneTool

} // namespace G4UA

#endif
