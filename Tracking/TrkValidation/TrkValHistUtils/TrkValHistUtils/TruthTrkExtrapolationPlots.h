/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_TRUTHTRKEXTRAPOLATIONPLOTS_H
#define TRKVALHISTUTILS_TRUTHTRKEXTRAPOLATIONPLOTS_H

#include "PlotBase.h"
#include "xAODTruth/TruthParticle.h"
#include "TVector3.h"

namespace Trk{

class TruthTrkExtrapolationPlots: public PlotBase {
  public:
    TruthTrkExtrapolationPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){ init();}
    void fill(const xAOD::TruthParticle& truthprt);

    TH1* CaloEntry_p;
    TH1* MuonEntry_p;
    TH1* MuonExit_p;

  private:
    void init();
    void initializePlots();
    void FillPlots(TH1* hist, const xAOD::TruthParticle& truthprt,const std::string& sNom);

};

}

#endif

