/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_RECOINFOPLOTS_H
#define MUONHISTUTILS_RECOINFOPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/RecoInfoPlots.h"

#include "xAODMuon/Muon.h"

namespace Muon{

class RecoInfoPlots: public PlotBase {
  public:
    RecoInfoPlots(PlotBase *pParent, const std::string& sDir);

    void fill(const xAOD::Muon& mu, float weight=1.0);

    Trk::RecoInfoPlots m_oTrkRecoInfoPlots; //ID trk chi^2, ndf
    Trk::RecoInfoPlots m_oMSTrkRecoInfoPlots; //MS trk chi^2, ndf
    Trk::RecoInfoPlots m_oRecoInfoPlots; //primary trk chi^2, ndf

    TH1* author;
    TH1* quality;
    TH1* quality_cutflow;
    TH1* muonType;

  
  private:
    void initializePlots();

};

}

#endif
