/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotFinder/BeamSpotStatusCode.h"

//bit settings for the status word components
using namespace InDet;

 const int  BeamSpotStatusCode::m_fitStatusBit = 0;
 const int  BeamSpotStatusCode::m_fitStatusLength = 2;
 const int  BeamSpotStatusCode::m_onlineBit = 2;
 const int  BeamSpotStatusCode::m_onlineLength = 1;
 const int  BeamSpotStatusCode::m_widthBit = 3; 
 const int  BeamSpotStatusCode::m_widthLength = 1;
 const int  BeamSpotStatusCode::m_algorithmTypeBit = 4;
 const int  BeamSpotStatusCode::m_algorithmTypeLength = 4;
