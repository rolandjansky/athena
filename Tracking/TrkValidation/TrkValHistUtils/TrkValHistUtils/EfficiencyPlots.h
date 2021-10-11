/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_EFFICIENCYPLOTS_H
#define TRKVALHISTUTILS_EFFICIENCYPLOTS_H

#include "PlotBase.h"
#include "ParamPlots.h"
#include "TH1.h"
#include "TH2.h"

namespace Trk{

class EfficiencyPlots:public PlotBase {
  public:   
    EfficiencyPlots(PlotBase* pParent, const std::string& sDir, ParamPlots* pNumPlots, ParamPlots* pDenomPlots);
        
    ParamPlots* m_pDenomPlots;
    ParamPlots* m_pNumPlots;    

    TH1* eff_eta;
    TH1* eff_phi;
    TH1* eff_pt;
    TH1* eff_pti;

  private:
    virtual void initializePlots();
    virtual void finalizePlots();

};
}

#endif // TRKVALHISTUTILS_EFFICIENCYPLOTS_H
