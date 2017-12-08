/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "PhysicsValidationUserActionTool.h"

namespace G4UA{
  
  namespace iGeant4{
    
    PhysicsValidationUserActionTool::PhysicsValidationUserActionTool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<PhysicsValidationUserAction>(type, name, parent), m_config(){
      
      
      declareProperty( "ValidationOutput",
		       m_config.validationOutput,
		       "If turned on, write out a ROOT tree.");
      declareProperty("ValidationStreamName",
		      m_config.validationStream,
		      "Name of the output stream" );
      declareProperty("THistService",
		      m_config.thistSvc,
		      "The THistSvc" );
      declareProperty("ParticleBroker", m_config.particleBroker, "ISF Particle Broker Svc");
      declareProperty("ParticleHelper", m_config.particleHelper, "ISF Particle Helper"    );
      declareProperty("GeoIDSvc"      , m_config.geoIDSvc      , "ISF GeoID Svc"          );
      declareProperty("UserActionSvc",m_config.UASvc);
      declareProperty("ID_Rmax",m_config.idR);
      declareProperty("ID_Zmax",m_config.idZ);
      declareProperty("CaloRmean",m_config.caloRmean);
      declareProperty("CaloZmean",m_config.caloZmean);
      declareProperty("MuonRmean",m_config.muonRmean);
      declareProperty("MuonZmean",m_config.muonZmean);
      declareProperty("CavernRmean",m_config.cavernRmean);
      declareProperty("CavernZmean",m_config.cavernZmean);
      
    }
    
    std::unique_ptr<PhysicsValidationUserAction>  PhysicsValidationUserActionTool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = CxxUtils::make_unique<PhysicsValidationUserAction>(m_config);
      return std::move(action);
    }

    StatusCode PhysicsValidationUserActionTool::queryInterface(const InterfaceID& riid, void** ppvIf)
    {
      if(riid == IG4EventActionTool::interfaceID()) {
        *ppvIf = (IG4EventActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IG4RunActionTool::interfaceID()) {
        *ppvIf = (IG4RunActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IG4SteppingActionTool::interfaceID()) {
        *ppvIf = (IG4SteppingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      if(riid == IG4TrackingActionTool::interfaceID()) {
        *ppvIf = (IG4TrackingActionTool*) this;
        addRef();
        return StatusCode::SUCCESS;
      }
      return ActionToolBase<PhysicsValidationUserAction>::queryInterface(riid, ppvIf);
    }

  } // iGeant4

} // namespace G4UA
