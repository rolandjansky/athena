/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONSEGMENTRESOLUTIONPLOTS_H
#define MUONHISTUTILS_MUONSEGMENTRESOLUTIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/MuonSegment.h"
#include <vector>
#include <string>

namespace Muon{

class MuonSegmentResolutionPlots:public PlotBase {
    public:
      MuonSegmentResolutionPlots(PlotBase* pParent, std::string sDir);
      ~MuonSegmentResolutionPlots();

      void fill(const xAOD::MuonSegment& muonSeg,const xAOD::MuonSegment& truthMuonSeg);
      
      TH1* deltaPosX;
      TH1* deltaPosY;
      TH1* deltaPosZ;
      TH1* deltaPosEta;
      TH1* deltaPosPhi;

      TH1* deltaDirPhi;
      TH1* deltaDirEta;

      //      std::vector<TH1*> sector_perStation;


};
}

#endif
