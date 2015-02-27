/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONTREE_H
#define MUONHISTUTILS_MUONTREE_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

#include "TTree.h"

namespace Muon{

  class MuonTree:public PlotBase {
  public:
    MuonTree(PlotBase* pParent, std::string sDir);
    MuonTree();

    TTree* tree;
    void fillTree(const xAOD::Muon& muon, const xAOD::TrackParticle* muontpIP, const xAOD::TruthParticle& truthprt);
    void initializePlots();

  private:
    float pCB;
    float pMS;
    float pMExtrapol;
    float pTruth;
    float pTruthMS;
    float etaCB;
    float etaMS;
    float etaMExtrapol;
    float etaTruth;
    float etaTruthMS;
    float phiCB;
    float phiMS;
    float phiMExtrapol;
    float phiTruth;
    float phiTruthMS;
    float elossMeasured;

  };

}

#endif
