/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_DEFPARAMPULLPLOTS_H
#define TRKVALHISTUTILS_DEFPARAMPULLPLOTS_H

#include "PlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

namespace Trk{

class DefParamPullPlots: public PlotBase {
  public:
    DefParamPullPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){ init();}
    void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);

    TH1* Pull_d0;
    TH1* Pull_z0;
    TH1* Pull_theta;
    TH1* Pull_phi0;
    TH1* Pull_qOverP;
    TH1* Pull_P;

    TH2* Pull_d0_vs_pt;
    TH2* Pull_z0_vs_pt;
    TH2* Pull_theta_vs_pt;
    TH2* Pull_phi0_vs_pt;
    TH2* Pull_qOverP_vs_pt;
    TH2* Pull_P_vs_pt;

  private:
    void init();
    void initializePlots();
};

}
#endif
