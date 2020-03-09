/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_EFFICIENCYPTPLOTS_H
#define TAUHISTUTILS_EFFICIENCYPTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TauHistUtils/ParamPlots.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauDefs.h"

namespace Tau{

class EfficiencyPtPlots: public PlotBase {
  public:
    EfficiencyPtPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
    virtual ~EfficiencyPtPlots();

    void fill(const xAOD::TauJet& tau);
    TProfile* m_eff_pt_jetBDTloose;
    TProfile* m_eff_pt_jetBDTmed;
    TProfile* m_eff_pt_jetBDTtight;
    TProfile* m_eff_pt_jetBDTlooseHighPt;
    TProfile* m_eff_pt_jetBDTmedHighPt;
    TProfile* m_eff_pt_jetBDTtightHighPt;

    TProfile* m_eff_jetBDTloose;
    TProfile* m_eff_jetBDTmed;
    TProfile* m_eff_jetBDTtight;

  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
