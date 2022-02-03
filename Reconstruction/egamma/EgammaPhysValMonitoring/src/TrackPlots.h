/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_TRACKPLOTS_H
#define EGAMMAPHYSVALMONITORING_TRACKPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "EgammaPhysValHistUtilities.h"
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

namespace Egamma{
  
class TrackPlots:public PlotBase {
    public:
      TrackPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
      void fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo) const;
     
      std::string m_sParticleType;
        
      TH1* deta;
      TH1* dphi;
      TH1* d0;
      TH1* z0;
      TH1* d0significance;
      TH1* blayer; 
      TH1* pixel;
      TH1* sct;
      TH1* si;
      TH1* trt;
      TH1* trt_xe;
      TH1* trt_total;
      TH1* trt_ht;
      TH1* trt_ht_total;
      TH1* dphirescaled;
      TH1* eProbHT;
      TH1* deltaPoverP;
      TH1* EoverP;
      TH2* trtratio;
      TH2* trtvseta;
      TH2* trthtvseta;

    private:
      virtual void initializePlots();
      
};

}

#endif
