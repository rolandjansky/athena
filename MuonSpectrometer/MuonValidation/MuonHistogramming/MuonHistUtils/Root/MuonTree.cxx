/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonTree.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "TROOT.h"

namespace Muon{

  MuonTree::MuonTree(PlotBase* pParent, std::string sDir):
    PlotBase(pParent, sDir),
    tree(NULL),
    m_pCB(-999.),
    m_pMS(-999.),
    m_pMExtrapol(-999.),
    m_pTruth(-999.),
    m_pTruthMS(-999.),
    m_etaCB(-999.),
    m_etaMS(-999.),
    m_etaMExtrapol(-999.),
    m_etaTruth(-999.),
    m_etaTruthMS(-999.),
    m_phiCB(-999.),
    m_phiMS(-999.),
    m_phiMExtrapol(-999.),
    m_phiTruth(-999.),
    m_phiTruthMS(-999.),
    m_elossMeasured(-999.)
  {}
  
  void MuonTree::initializePlots()
  {
    tree = BookTree("MuonTree");
    
    tree->Branch("pCB", &m_pCB, "pCB/F");
    tree->Branch("pMS", &m_pMS, "pMS/F");
    tree->Branch("pMExtrapol", &m_pMExtrapol, "pMExtrapol/F");
    tree->Branch("pTruth", &m_pTruth, "pTruth/F");
    tree->Branch("pTruthMS", &m_pTruthMS, "pTruthMS/F");

    tree->Branch("etaCB", &m_etaCB, "etaCB/F");
    tree->Branch("etaMS", &m_etaMS, "etaMS/F");
    tree->Branch("etaMExtrapol", &m_etaMExtrapol, "etaMExtrapol/F");
    tree->Branch("etaTruth", &m_etaTruth, "etaTruth/F");
    tree->Branch("etaTruthMS", &m_etaTruthMS, "etaTruthMS/F");
    tree->Branch("phiCB", &m_phiCB, "phiCB/F");
    tree->Branch("phiMS", &m_phiMS, "phiMS/F");
    tree->Branch("phiMExtrapol", &m_phiMExtrapol, "phiMExtrapol/F");
    tree->Branch("phiTruth", &m_phiTruth, "phiTruth/F");
    tree->Branch("phiTruthMS", &m_phiTruthMS, "phiTruthMS/F");

    tree->Branch("elossMeasured", &m_elossMeasured, "elossMeasured/F");
  }

  void MuonTree::fillTree(const xAOD::Muon& muon, const xAOD::TrackParticle* msTrk, const xAOD::TruthParticle& truthMu)
  {
    const xAOD::TrackParticle* muPrimaryTrk = muon.trackParticle(xAOD::Muon::Primary);
    if (!muPrimaryTrk) return;

    //muon extrapolated to IP
    ////////////////// @@@ sorting out the mess with the link to the extrapolated muon
    //for 20.1.0...
    /// const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle); // points to the ExtrapolatedMuonSpectrometerTrackParticle, the ExtrapolatedMuonSpectrometerTrackParticle link doesn't exist

    //for 20.1.3...
    //const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

    //trying to accomodate both in a way that the code compiles in both releases
    int correctEnum = (int) xAOD::Muon::MuonSpectrometerTrackParticle;
    if (muon.isAvailable< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink") && (muon.auxdata<ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink")).isValid()) correctEnum+=2; //check correct numbering in Muon.h
    const xAOD::TrackParticle* msExtrapTrk = muon.trackParticle((xAOD::Muon::TrackParticleType) correctEnum);
      
    m_pTruth = truthMu.p4().P();
    m_pCB = muPrimaryTrk->p4().P();

    m_etaTruth = truthMu.p4().Eta();
    m_etaCB = muPrimaryTrk->p4().Eta();

    m_phiTruth = truthMu.p4().Phi();
    m_phiCB = muPrimaryTrk->p4().Phi();


    m_pMExtrapol = 0;
    if (msExtrapTrk) {
      m_pMExtrapol = msExtrapTrk->p4().P();
      m_etaMExtrapol = msExtrapTrk->p4().Eta();
      m_phiMExtrapol = msExtrapTrk->p4().Phi();
    }
    else{
      m_pMExtrapol = -999;
      m_etaMExtrapol = -999;
      m_phiMExtrapol = -999;
    }
    //muon extrapolated
    m_pMS = 0;
    if (msTrk) {
      m_pMS = msTrk->p4().P(); //at muon spectrometer entry//@@@
      m_etaMS = msTrk->p4().Eta(); //at muon spectrometer entry//@@@
      m_phiMS = msTrk->p4().Phi(); //at muon spectrometer entry//@@@
    }
    else{
      m_pMS = -999; //at muon spectrometer entry//@@@
      m_etaMS = -999; //at muon spectrometer entry//@@@
      m_phiMS = -999; //at muon spectrometer entry//@@@
    }

#ifndef XAOD_ANALYSIS
    // measured E-loss
    if (!muon.parameter(m_elossMeasured,xAOD::Muon::MeasEnergyLoss)) {
      m_elossMeasured = -999.;
    }
#endif // not XAOD_ANALYSIS
    
    m_pTruthMS = 0; //p truth at MS entry
    if (truthMu.isAvailable<float>("MuonEntryLayer_px") &&
	truthMu.isAvailable<float>("MuonEntryLayer_py") &&
	truthMu.isAvailable<float>("MuonEntryLayer_pz") ) {
      TVector3 pvecTruthMS(truthMu.auxdata<float>("MuonEntryLayer_px"),
			   truthMu.auxdata<float>("MuonEntryLayer_py"),
			   truthMu.auxdata<float>("MuonEntryLayer_pz"));
      m_pTruthMS = pvecTruthMS.Mag();
      m_etaTruthMS = pvecTruthMS.Eta();
      m_phiTruthMS = pvecTruthMS.Phi();
    }
    else{
      m_pTruth = -999;
      m_etaTruth = -999;
      m_phiTruth = -999;
    }

    tree->Fill();
  }
   
} // closing namespace Muon
