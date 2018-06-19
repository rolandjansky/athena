/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__MCTRUTHUSERACTIONTOOL_H
#define ISF_GEANT4TOOLS_G4UA__MCTRUTHUSERACTIONTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "MCTruthUserAction.h"

namespace G4UA
{

  namespace iGeant4
  {

    /// @class MCTruthUserActionTool
    /// @brief Tool which manages the MCTruthUserAction
    ///
    /// @author Andrea Di Simone
    ///

    class MCTruthUserActionTool : public UserActionToolBase<MCTruthUserAction>
    {

    public:
      /// Standard constructor
      MCTruthUserActionTool(const std::string& type, const std::string& name,
                            const IInterface* parent);
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<MCTruthUserAction>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// Configuration parameters
      MCTruthUserAction::Config m_config;
    }; // class MCTruthUserActionTool

  }// iGeant4

} // namespace G4UA

#endif
