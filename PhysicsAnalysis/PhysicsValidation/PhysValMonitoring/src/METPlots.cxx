/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>



#include "METPlots.h"

using CLHEP::GeV;

namespace PhysVal{

  METPlots::METPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir),
m_sParticleType(std::move(sParticleType))
{}	
  
void METPlots::initializePlots(){
  met_ex  = Book1D("Ex", "MET " + m_sParticleType + " Ex; E_{x}^{miss} ;Events", 100, 0., 400);
  met_ey  = Book1D("Ey", "MET " + m_sParticleType + " Ey; E_{y}^{miss} ;Events", 100, 0., 400);
  met_et  = Book1D("Et", "MET " + m_sParticleType + " Et; E_{t}^{miss} ;Events", 100, 0., 400);
  met_sumet  = Book1D("SumEt", "MET " + m_sParticleType + " Ex; #Sum E_{t} ;Events", 100, 0., 2000);
}

  void METPlots::fill(const xAOD::MissingET* met,const xAOD::EventInfo* evt) const{
    
    met_ex->Fill(met->mpx()/GeV,evt->beamSpotWeight());
    met_ey->Fill(met->mpy()/GeV,evt->beamSpotWeight());
    met_et->Fill(met->met()/GeV,evt->beamSpotWeight());
    met_sumet->Fill(met->sumet()/GeV,evt->beamSpotWeight());

}
}
