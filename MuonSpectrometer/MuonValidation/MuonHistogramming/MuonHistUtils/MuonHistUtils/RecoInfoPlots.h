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
    RecoInfoPlots(PlotBase *pParent, std::string sDir);

    void fill(const xAOD::Muon& mu);

    Trk::RecoInfoPlots m_oTrkRecoInfoPlots; //ID trk chi^2, ndf
    Trk::RecoInfoPlots m_oMSTrkRecoInfoPlots; //MS trk chi^2, ndf
    Trk::RecoInfoPlots m_oRecoInfoPlots; //primary trk chi^2, ndf

    TH1* author;
    TH1* quality;
    TH1* muonType;

    TH1* innerMatchingChi2;
    TH1* innerMatchingNdof;
    TH1* innerMatchingCon;

    TH1* outerMatchingChi2;
    TH1* outerMatchingNdof;
    TH1* outerMatchingCon;


  private:
    void initializePlots();
    void fillMatchingPlots(TH1* matchingChi2, TH1* matchingNdof, TH1* matchingCon, xAOD::Muon::ParamDef eChi2, xAOD::Muon::ParamDef eNdof, const xAOD::Muon& mu);

};

}

#endif
