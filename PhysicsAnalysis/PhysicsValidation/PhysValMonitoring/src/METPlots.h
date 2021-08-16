/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    METPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType = "RefFinal");
    void fill(const xAOD::MissingET* met,const xAOD::EventInfo* evt) const;
      
      // Reco only information
      std::string m_sParticleType;
      TH1* met_ex = nullptr;
      TH1* met_ey = nullptr;
      TH1* met_et = nullptr;
      TH1* met_sumet = nullptr;

    private:
      virtual void initializePlots();      
};

}

#endif
