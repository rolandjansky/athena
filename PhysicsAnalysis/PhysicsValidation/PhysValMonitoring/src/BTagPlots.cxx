/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>



#include "BTagPlots.h"

using CLHEP::GeV;

namespace PhysVal{

  BTagPlots::BTagPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir),
m_sParticleType(std::move(sParticleType))
{}	
  
void BTagPlots::initializePlots(){
  n  = Book1D("n", "Number of b jets from "+ m_sParticleType +"; n ;Events", 10, 0., 10);
}

  void BTagPlots::fill(const xAOD::BTagging* /*btag*/, const xAOD::EventInfo* evt){
    std::cout << "filling b-tagging plots with BS weight: " << evt->beamSpotWeight();
}

  void BTagPlots::fill(unsigned int nbtag,const xAOD::EventInfo* evt) const{
    n->Fill(nbtag,evt->beamSpotWeight());
}
}
