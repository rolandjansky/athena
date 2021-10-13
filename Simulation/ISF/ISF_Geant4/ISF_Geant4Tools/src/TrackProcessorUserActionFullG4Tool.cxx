/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackProcessorUserActionFullG4Tool.h"

namespace G4UA
{

  namespace iGeant4
  {

    TrackProcessorUserActionFullG4Tool::
    TrackProcessorUserActionFullG4Tool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
      : UserActionToolBase<TrackProcessorUserActionFullG4>(type, name, parent),
        m_config(name)
    {
      declareProperty("EntryLayerTool",   m_config.entryLayerTool, "ISF Entry Layer Tool");
      declareProperty("GeoIDSvc",         m_config.geoIDSvc,       "ISF GeoIDService"                            );
      declareProperty("TruthVolumeLevel", m_config.truthVolLevel,  "Level in geo hierarchy for the truth volumes");
      declareProperty("IsITkGeometry",    m_config.isITkGeometry,  "toggle for ITk");
    }

    std::unique_ptr<TrackProcessorUserActionFullG4>
    TrackProcessorUserActionFullG4Tool::makeAndFillAction(G4AtlasUserActions& actionList)
    {
      ATH_MSG_DEBUG("Constructing a TrackProcessorUserActionFullG4");
      if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
      else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
      auto action = std::make_unique<TrackProcessorUserActionFullG4>(m_config);
      actionList.eventActions.push_back( action.get() );
      actionList.trackingActions.push_back( action.get() );
      actionList.steppingActions.push_back( action.get() );
      return action;
    }

  } // iGeant4

} // namespace G4UA
