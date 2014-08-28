/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureUtils/Charge.h"

using namespace std;
using namespace JetSubStructureUtils;

double Charge::result(const xAOD::Jet &jet) const
{
  vector<const xAOD::TrackParticle*> tracks;
  bool retval = jet.getAssociatedObjects("GhostTrack", tracks);
  if(retval == false) {
    return -999;
  }

  double charge = 0;
  for(unsigned int i=0; i<tracks.size(); i++) {
    charge += tracks[i]->charge() * pow(tracks[i]->pt(), m_k);
  }
  if(jet.pt() == 0) {
    return -999;
  }
  else {
    charge /= pow(jet.pt(), m_k);
  }

  return charge;
}
