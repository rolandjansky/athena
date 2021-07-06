/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_METPLOTS_H
#define PHYSVALMONITORING_METPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODMissingET/MissingET.h"
#include "xAODEventInfo/EventInfo.h"

namespace PhysVal{
  
class METPlots:public PlotBase {
    public:
    METPlots(PlotBase* pParent, std::string sDir, std::string sParticleType = "RefFinal");
    void fill(const xAOD::MissingET* met,const xAOD::EventInfo* evt);
      
      // Reco only information
      std::string m_sParticleType;
      TH1* met_ex;
      TH1* met_ey;
      TH1* met_et;
      TH1* met_sumet;

    private:
      virtual void initializePlots();      
};

}

#endif
