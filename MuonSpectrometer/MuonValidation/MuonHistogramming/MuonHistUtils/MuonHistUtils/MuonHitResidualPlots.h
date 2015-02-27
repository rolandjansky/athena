/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONHITRESIDUALPLOTS_H
#define MUONHISTUTILS_MUONHITRESIDUALPLOTS_H

#include "TrkValHistUtils/HitResidualPlots.h"
#include "TrkEventPrimitives/ResidualPull.h"

namespace Muon{
 
class MuonHitResidualPlots: public PlotBase {
    public:
      MuonHitResidualPlots(PlotBase *pParent, std::string sDir, std::string sType="");
      void fill(const Trk::ResidualPull& resPull, int stationPhi );

      Trk::HitResidualPlots m_oResidualPlots;

      TH2* pulls_vs_sectorPhi;
      TProfile* pullsRMS_vs_sectorPhi;
	      
    private:
      void init();
  };

}

#endif
