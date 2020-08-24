/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "InDetTestBLayer/TrackStateOnBLayerInfo.h"


namespace InDet {
  

  InDet::TrackStateOnBLayerInfo::TrackStateOnBLayerInfo(){

    m_type = unknownBLayerTrackState;
    m_moduleId = 0; 
    m_pixelId = 0;
    m_locX= -99999;
    m_locY= -99999;
    m_errLocX= -99999;
    m_errLocY= -99999;
    m_theta= -99999;
    m_phi= -99999;
    m_distToModEdgePhi= -99999;
    m_distToModEdgeEta= -99999;
    m_goodFrac = -99999;
  }


} //end namespace

