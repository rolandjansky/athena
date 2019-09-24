/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MyAlgTool.h"

MyAlgTool::MyAlgTool(const std::string& t, const std::string& n,  const IInterface* p ) :
AthAlgTool(t,n,p)
{
  declareInterface<IMyAlgTool>(this);
}

bool MyAlgTool::selectTrackParticle( const xAOD::TrackParticle& particle) const {
  return particle.pt()>2.0;
}
