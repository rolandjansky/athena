/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAndVtxPlots.h"

using CLHEP::GeV;

namespace PhysVal{

TrkAndVtxPlots::TrkAndVtxPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
{}	
  
void TrkAndVtxPlots::initializePlots(){
  ntrk  = Book1D("ntrk", "Number of tracks; n ;Events", 1000, 0., 1000);
  nvtx  = Book1D("nvtx", "Number of vertices; n ;Events", 50, 0., 50);

  vtx_x  = Book1D("x", "Vertex x; x ;Events", 200, -1., 1);
  vtx_y  = Book1D("y", "Vertex y; y ;Events", 300, -1.5, 1.5);
  vtx_z  = Book1D("z", "Vertex z; z ;Events", 200, -250., 250);

  mu  = Book1D("mu", "Pileup; mu ;Events", 120, 0., 120);
}

void TrkAndVtxPlots::fill(const xAOD::Vertex* vtx){

  vtx_x->Fill(vtx->x());
  vtx_y->Fill(vtx->y());
  vtx_z->Fill(vtx->z());

}

  void TrkAndVtxPlots::fill(const xAOD::TrackParticle* /*trk*/){

}

  void TrkAndVtxPlots::fill(unsigned int ntrack, unsigned int nvertex, float pileup){
  ntrk->Fill(ntrack);
  nvtx->Fill(nvertex);
  mu->Fill(pileup);
}

}
