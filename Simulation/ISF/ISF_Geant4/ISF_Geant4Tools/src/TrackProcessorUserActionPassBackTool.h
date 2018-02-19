/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H
#define ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "TrackProcessorUserActionPassBack.h"

namespace G4UA
{

  namespace iGeant4
  {

    /// @class TrackProcessorUserActionPassBackTool
    /// @brief Tool which manages the TrackProcessorUserActionPassBack
    ///
    /// @author Andrea Di Simone
    ///
    class TrackProcessorUserActionPassBackTool :
      public UserActionToolBase<TrackProcessorUserActionPassBack>
    {

    public:
      /// Standard constructor
      TrackProcessorUserActionPassBackTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<TrackProcessorUserActionPassBack>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// Configuration parameters
      TrackProcessorUserActionPassBack::Config m_config;
    }; // class TrackProcessorUserActionPassBackTool

  }  // iGeant4

} // namespace G4UA

#endif
