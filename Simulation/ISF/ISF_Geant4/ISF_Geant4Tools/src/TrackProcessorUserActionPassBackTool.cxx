/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrackProcessorUserActionPassBackTool.h"
#include "ISF_Interfaces/IParticleBroker.h"

namespace G4UA{

  namespace iGeant4{

    TrackProcessorUserActionPassBackTool::TrackProcessorUserActionPassBackTool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<TrackProcessorUserActionPassBack>(type, name, parent), m_config(){

      declareProperty("ParticleBroker", m_config.particleBroker, "ISF Particle Broker Svc");
      declareProperty("GeoIDSvc"      , m_config.geoIDSvc      , "ISF GeoID Svc"          );

      declareProperty("PassBackEkinThreshold",
                      m_config.passBackEkinThreshold=0.05, //TODO: should we add units here (MeV)?
                      "Ekin cut-off for particles returned to ISF");
      declareProperty("KillBoundaryParticlesBelowThreshold",
                      m_config.killBoundaryParticlesBelowThreshold=false,
                      "Kill particles at boundary which are below Ekin cut-off rather than continue their simulation in G4");

    }

    std::unique_ptr<TrackProcessorUserActionPassBack>  TrackProcessorUserActionPassBackTool::makeAction(){

      ATH_MSG_DEBUG("makeAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = CxxUtils::make_unique<TrackProcessorUserActionPassBack>(m_config);
      return std::move(action);
    }

    StatusCode TrackProcessorUserActionPassBackTool::queryInterface(const InterfaceID& riid, void** ppvIf){

      if(riid == IG4TrackingActionTool::interfaceID()) {
        *ppvIf = (IG4TrackingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IG4SteppingActionTool::interfaceID()) {
        *ppvIf = (IG4SteppingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IG4EventActionTool::interfaceID()) {
        *ppvIf = (IG4EventActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      return ActionToolBase<TrackProcessorUserActionPassBack>::queryInterface(riid, ppvIf);
    }

  } // iGeant4
} // namespace G4UA
