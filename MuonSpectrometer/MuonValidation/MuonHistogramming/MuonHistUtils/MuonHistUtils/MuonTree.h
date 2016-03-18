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
    float m_pCB;
    float m_pMS;
    float m_pMExtrapol;
    float m_pTruth;
    float m_pTruthMS;
    float m_etaCB;
    float m_etaMS;
    float m_etaMExtrapol;
    float m_etaTruth;
    float m_etaTruthMS;
    float m_phiCB;
    float m_phiMS;
    float m_phiMExtrapol;
    float m_phiTruth;
    float m_phiTruthMS;
    float m_elossMeasured;

  };

}

#endif
