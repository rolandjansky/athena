/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#ifndef MUONHISTUTILS_MUONHITRESIDUALPLOTS_H
#define MUONHISTUTILS_MUONHITRESIDUALPLOTS_H

#include <string>
#include "TrkValHistUtils/HitResidualPlots.h"
#include "TrkValHistUtils/PlotBase.h"

//forward declarations
class TH2;
class TProfile;
namespace Trk
{ 
  class ResidualPull;
}

namespace Muon{
 
class MuonHitResidualPlots: public PlotBase {
    public:
      MuonHitResidualPlots(PlotBase *pParent, const std::string& sDir, std::string sType="");
      void fill(const Trk::ResidualPull& resPull, int stationPhi );
      
      Trk::HitResidualPlots m_oResidualPlots;      
      TH2* pulls_vs_sectorPhi{nullptr};
      TProfile* pullsRMS_vs_sectorPhi{nullptr};
      
    private:
      void init();
  };

}

#endif
#endif // not XAOD_ANALYSIS
