/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/TrkValHistUtilities.h"

namespace Trk{

uint8_t RetrieveHitInfo(const xAOD::TrackParticle& trkprt,const xAOD::SummaryType& info){
  uint8_t val = 0;
  if (!trkprt.summaryValue(val,info)) val = 0;
  return val;
}

}
