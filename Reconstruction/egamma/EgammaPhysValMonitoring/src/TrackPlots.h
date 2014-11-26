/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_TRACKPLOTS_H
#define EGAMMAPHYSVALMONITORING_TRACKPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "EgammaPhysValHistUtilities.h"
#include "xAODEgamma/Electron.h"

namespace Egamma{
  
class TrackPlots:public PlotBase {
    public:
      TrackPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Electron& electron);
     
      std::string m_sParticleType;
    
      TH1* deta;
      TH1* dphi;
      TH1* blayer; 
      TH1* pixel;
      TH1* sct;
      TH1* si;
      TH1* trt;
      TH2* trtratio;

    private:
      virtual void initializePlots();
      
};

}

#endif
