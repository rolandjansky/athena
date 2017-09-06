/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrackProcessorUserActionFullG4Tool.h"
namespace G4UA{

  namespace iGeant4{

    TrackProcessorUserActionFullG4Tool::TrackProcessorUserActionFullG4Tool(const std::string& type, const std::string& name,const IInterface* parent):
      ActionToolBase<TrackProcessorUserActionFullG4>(type, name, parent), m_config(name){

      declareProperty("EntryLayerTool",   m_config.entryLayerTool, "ISF Entry Layer Tool");
      declareProperty("GeoIDSvc",         m_config.geoIDSvc,       "ISF GeoIDService"                            );
      declareProperty("TruthVolumeLevel", m_config.truthVolLevel,  "Level in geo hierarchy for the truth volumes");

    }

    std::unique_ptr<TrackProcessorUserActionFullG4>  TrackProcessorUserActionFullG4Tool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = CxxUtils::make_unique<TrackProcessorUserActionFullG4>(m_config);
      return std::move(action);
    }

    StatusCode TrackProcessorUserActionFullG4Tool::queryInterface(const InterfaceID& riid, void** ppvIf){

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
      return ActionToolBase<TrackProcessorUserActionFullG4>::queryInterface(riid, ppvIf);
    }
  } // iGeant4

} // namespace G4UA
