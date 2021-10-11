/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_TRKANDVTXPLOTS_H
#define PHYSVALMONITORING_TRKANDVTXPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODEventInfo/EventInfo.h"

namespace PhysVal{
  
class TrkAndVtxPlots:public PlotBase {
    public:
      TrkAndVtxPlots(PlotBase* pParent, const std::string& sDir);
      void fill(const xAOD::TrackParticle* trk,const xAOD::EventInfo* evt);
      void fill(const xAOD::Vertex* vtx,const xAOD::EventInfo* evt) const;
      void fill(unsigned int ntrack, unsigned int nvertex, float pileup = 0,const xAOD::EventInfo* evt=NULL) const;
      
      // Reco only information
      TH1* ntrk = nullptr;

      TH1* nvtx = nullptr;
      TH1* vtx_x = nullptr;
      TH1* vtx_y = nullptr;
      TH1* vtx_z = nullptr;

      TH1* mu = nullptr;

    private:
      virtual void initializePlots();

      
};

}

#endif
