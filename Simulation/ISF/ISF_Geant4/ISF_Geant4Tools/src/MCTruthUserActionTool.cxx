/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthUserActionTool.h"

namespace G4UA
{

  namespace iGeant4
  {

    MCTruthUserActionTool::MCTruthUserActionTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
      : UserActionToolBase<MCTruthUserAction>(type, name, parent)
    {
      declareProperty("TruthRecordSvc", m_config.truthRecordSvc,
                      "ISF Particle Truth Svc");
      declareProperty("SecondarySavingLevel", m_config.ilevel=2);
    }

    std::unique_ptr<MCTruthUserAction>
    MCTruthUserActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
    {
      ATH_MSG_DEBUG("Constructing an MCTruthUserAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = std::make_unique<MCTruthUserAction>(m_config);
      actionList.trackingActions.push_back( action.get() );
      return action;
    }

  } // iGeant4

} // namespace G4UA
