/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/SlowMuonParamPlots.h"

namespace Muon{
											   
SlowMuonParamPlots::SlowMuonParamPlots(PlotBase *pParent, std::string sDir)
  :PlotBase(pParent, sDir)
  , beta(NULL)
{}

void SlowMuonParamPlots::initializePlots()
{
  beta = Book1D("beta", "beta;beta;Entries",150,0,1.5);  
}

void SlowMuonParamPlots::fill(const xAOD::SlowMuon& smu)
{
  beta->Fill( smu.beta() );
}  

} //close namespace Muon
