/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "MuonHistUtils/MuonHitResidualPlots.h"
#include <stddef.h> // for NULL
#include <vector>
#include "TH2.h" 
#include "TProfile.h" 
#include "TrkEventPrimitives/ResidualPull.h"

namespace Muon{
											   
MuonHitResidualPlots::MuonHitResidualPlots(PlotBase *pParent, const std::string& sDir,std::string sType):PlotBase(pParent, sDir)
, m_oResidualPlots(this, "",sType)
, pulls_vs_sectorPhi(nullptr)
, pullsRMS_vs_sectorPhi(nullptr)
{
  if (sType!="") sType=sType+"_";
  pulls_vs_sectorPhi = Book2D(sType+"pulls_vs_sectorPhi",sType+" pulls vs sectorPhi",16,1,17,50,-20,20);
  pullsRMS_vs_sectorPhi = BookTProfile(sType+"pullsRMS_sectorPhi",sType+" pullsRMS vs sectorPhi",16,1,17,-20,20); 
}

void MuonHitResidualPlots::fill(const Trk::ResidualPull& resPull, int stationPhi )
{
  m_oResidualPlots.fill(resPull);

  //const float residual = resPull.residual().front();
  const float pull = resPull.pull().front();

  pulls_vs_sectorPhi->Fill(stationPhi,pull);
  pullsRMS_vs_sectorPhi->Fill(stationPhi,pull);
}

}//closing namespace

#endif // not XAOD_ANALYSIS

