/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_CHARGEDEPPARAMPLOTS_H
#define MUONHISTUTILS_CHARGEDEPPARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"

namespace Muon{

class ChargeDepParamPlots:public PlotBase {
  public:      
    ChargeDepParamPlots(PlotBase *pParent, const std::string& sDir, std::string sType);
    //void fill(const xAOD::TrackParticle& trkprt);
    void fill(const xAOD::Muon& mu,float weight=1.0);

    TH1* q_times_eta{nullptr};
    TH1* q_over_p{nullptr};
    TH1* q_over_pt{nullptr};
    TH1* qMS_times_qCB{nullptr};
    TH1* qID_times_qCB{nullptr};

  private:
    void initializePlots();
    std::string m_sType;
};

}

#endif // MUONHISTUTILS_CHARGEDEPPARAMPLOTS_H

