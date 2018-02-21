/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONFULLG4TOOL_H
#define ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONFULLG4TOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TrackProcessorUserActionFullG4.h"

namespace G4UA
{

  namespace iGeant4
  {

    /// @class TrackProcessorUserActionFullG4Tool
    /// @brief Tool which manages the TrackProcessorUserActionFullG4
    ///
    /// @author Andrea Di Simone
    ///
    class TrackProcessorUserActionFullG4Tool :
      public UserActionToolBase<TrackProcessorUserActionFullG4>
    {

    public:
      /// Standard constructor
      TrackProcessorUserActionFullG4Tool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<TrackProcessorUserActionFullG4>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// Configuration parameters
      TrackProcessorUserActionFullG4::Config m_config;
    }; // class TrackProcessorUserActionFullG4Tool

  }// iGeant4

} // namespace G4UA

#endif
