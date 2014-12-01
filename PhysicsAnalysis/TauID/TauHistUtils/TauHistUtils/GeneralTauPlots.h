/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_GENERALTAUPLOTS_H
#define TAUHISTUTILS_GENERALTAUPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "xAODTau/TauJet.h"

namespace Tau{

class GeneralTauPlots: public PlotBase {
  public:
    GeneralTauPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);

    void fill(const xAOD::TauJet& tau);

    Trk::ParamPlots m_oParamPlots;
    TH1* m_tauCharge;
    TH1* m_tauNCoreTracks;
    TH1* m_tauNWideTracks;
    
  private:
    void initializePlots();
    std::string m_sTauJetContainerName;
};

}

#endif
