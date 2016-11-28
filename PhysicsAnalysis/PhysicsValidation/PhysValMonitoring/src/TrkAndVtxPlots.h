/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_TRKANDVTXPLOTS_H
#define PHYSVALMONITORING_TRKANDVTXPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

namespace PhysVal{
  
class TrkAndVtxPlots:public PlotBase {
    public:
      TrkAndVtxPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::TrackParticle* trk);
      void fill(const xAOD::Vertex* vtx);
      void fill(unsigned int ntrack, unsigned int nvertex, float pileup = 0);
      
      // Reco only information
      TH1* ntrk;

      TH1* nvtx;
      TH1* vtx_x;
      TH1* vtx_y;
      TH1* vtx_z;

      TH1* mu;

    private:
      virtual void initializePlots();

      
};

}

#endif
