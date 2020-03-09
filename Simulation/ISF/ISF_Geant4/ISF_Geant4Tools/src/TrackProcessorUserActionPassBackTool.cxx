/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackProcessorUserActionPassBackTool.h"
#include "ISF_Interfaces/IParticleBroker.h"

namespace G4UA
{

  namespace iGeant4
  {

    TrackProcessorUserActionPassBackTool::
    TrackProcessorUserActionPassBackTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
      : UserActionToolBase<TrackProcessorUserActionPassBack>(type, name, parent)
    {
      declareProperty("ParticleBroker", m_config.particleBroker, "ISF Particle Broker Svc");
      declareProperty("GeoIDSvc"      , m_config.geoIDSvc      , "ISF GeoID Svc"          );

      declareProperty("PassBackEkinThreshold",
                      m_config.passBackEkinThreshold=0.05, //TODO: should we add units here (MeV)?
                      "Ekin cut-off for particles returned to ISF");
      declareProperty("KillBoundaryParticlesBelowThreshold",
                      m_config.killBoundaryParticlesBelowThreshold=false,
                      "Kill particles at boundary which are below Ekin cut-off rather than continue their simulation in G4");
    }

    std::unique_ptr<TrackProcessorUserActionPassBack>
    TrackProcessorUserActionPassBackTool::
    makeAndFillAction(G4AtlasUserActions& actionList)
    {
      ATH_MSG_DEBUG("Constructing a TrackProcessorUserActionPassBack");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = std::make_unique<TrackProcessorUserActionPassBack>(m_config);
      actionList.eventActions.push_back( action.get() );
      actionList.trackingActions.push_back( action.get() );
      actionList.steppingActions.push_back( action.get() );
      return action;
    }

  } // iGeant4

} // namespace G4UA
