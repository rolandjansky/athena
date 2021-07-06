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
    DefParamPullPlots(PlotBase *pParent, std::string sDir, std::string sType=""):PlotBase(pParent, sDir),m_sType(sType){ init();}
  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt, float weight=1.0);

    TH1* Pull_d0;
    TH1* Pull_z0;
    TH1* Pull_theta;
    TH1* Pull_phi;
    TH1* Pull_qOverP;

    TH2* Pull_d0_vs_pt;
    TH2* Pull_z0_vs_pt;
    TH2* Pull_theta_vs_pt;
    TH2* Pull_phi_vs_pt;
    TH2* Pull_qOverP_vs_pt;

  private:
    std::string m_sType;
    void init();
    void initializePlots();
};

}
#endif
