/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_RecoVertexPLOTS_H
#define MUONTRKPHYSMONITORING_RecoVertexPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODTracking/VertexContainer.h"


class RecoVertexPlots:public PlotBase {
  public:
    RecoVertexPlots(PlotBase *pParent, std::string sDir);

    void fill(const xAOD::Vertex& msVx);

    TH1* m_nMDT;
    TH1* m_nRPC;
    TH1* m_nTGC;
    TH1* m_nTracklets;
    TH2* m_VertexEtaPhi;

  private:
    void initializePlots();

};

#endif
