/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "InDetTestPixelLayer/TrackStateOnPixelLayerInfo.h"


namespace InDet {
  

  InDet::TrackStateOnPixelLayerInfo::TrackStateOnPixelLayerInfo(){

    m_type = unknownPixelLayerTrackState;
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

  InDet::TrackStateOnPixelLayerInfo::~TrackStateOnPixelLayerInfo(){
    ////....
  }




} //end namespace

