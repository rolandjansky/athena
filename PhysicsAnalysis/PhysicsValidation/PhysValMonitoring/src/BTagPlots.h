/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_BTagPLOTS_H
#define PHYSVALMONITORING_BTagPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEventInfo/EventInfo.h"

namespace PhysVal{
  
class BTagPlots:public PlotBase {
    public:
    BTagPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
    void fill(const xAOD::BTagging* btag,const xAOD::EventInfo* evt);
    void fill(unsigned int nbtag,const xAOD::EventInfo* evt) const;
      
      // Reco only information
      std::string m_sParticleType;
      TH1* n = nullptr;

    private:
      virtual void initializePlots();      
};

}

#endif
