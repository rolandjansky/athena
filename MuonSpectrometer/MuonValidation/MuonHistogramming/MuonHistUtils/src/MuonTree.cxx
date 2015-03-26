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
    pCB(-999.),
    pMS(-999.),
    pMExtrapol(-999.),
    pTruth(-999.),
    pTruthMS(-999.),
    etaCB(-999.),
    etaMS(-999.),
    etaMExtrapol(-999.),
    etaTruth(-999.),
    etaTruthMS(-999.),
    phiCB(-999.),
    phiMS(-999.),
    phiMExtrapol(-999.),
    phiTruth(-999.),
    phiTruthMS(-999.),
    elossMeasured(-999.)
  {}
  
  void MuonTree::initializePlots()
  {
    tree = BookTree("MuonTree");
    
    tree->Branch("pCB", &pCB, "pCB/F");
    tree->Branch("pMS", &pMS, "pMS/F");
    tree->Branch("pMExtrapol", &pMExtrapol, "pMExtrapol/F");
    tree->Branch("pTruth", &pTruth, "pTruth/F");
    tree->Branch("pTruthMS", &pTruthMS, "pTruthMS/F");

    tree->Branch("etaCB", &etaCB, "etaCB/F");
    tree->Branch("etaMS", &etaMS, "etaMS/F");
    tree->Branch("etaMExtrapol", &etaMExtrapol, "etaMExtrapol/F");
    tree->Branch("etaTruth", &etaTruth, "etaTruth/F");
    tree->Branch("etaTruthMS", &etaTruthMS, "etaTruthMS/F");
    tree->Branch("phiCB", &phiCB, "phiCB/F");
    tree->Branch("phiMS", &phiMS, "phiMS/F");
    tree->Branch("phiMExtrapol", &phiMExtrapol, "phiMExtrapol/F");
    tree->Branch("phiTruth", &phiTruth, "phiTruth/F");
    tree->Branch("phiTruthMS", &phiTruthMS, "phiTruthMS/F");

    tree->Branch("elossMeasured", &elossMeasured, "elossMeasured/F");
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
      
    pTruth = truthMu.p4().P();
    pCB = muPrimaryTrk->p4().P();

    etaTruth = truthMu.p4().Eta();
    etaCB = muPrimaryTrk->p4().Eta();

    phiTruth = truthMu.p4().Phi();
    phiCB = muPrimaryTrk->p4().Phi();


    pMExtrapol = 0;
    if (msExtrapTrk) {
      pMExtrapol = msExtrapTrk->p4().P();
      etaMExtrapol = msExtrapTrk->p4().Eta();
      phiMExtrapol = msExtrapTrk->p4().Phi();
    }
    else{
      pMExtrapol = -999;
      etaMExtrapol = -999;
      phiMExtrapol = -999;
    }
    //muon extrapolated
    pMS = 0;
    if (msTrk) {
      pMS = msTrk->p4().P(); //at muon spectrometer entry//@@@
      etaMS = msTrk->p4().Eta(); //at muon spectrometer entry//@@@
      phiMS = msTrk->p4().Phi(); //at muon spectrometer entry//@@@
    }
    else{
      pMS = -999; //at muon spectrometer entry//@@@
      etaMS = -999; //at muon spectrometer entry//@@@
      phiMS = -999; //at muon spectrometer entry//@@@
    }

    // measured E-loss
    if (!muon.parameter(elossMeasured,xAOD::Muon::MeasEnergyLoss)) {
      elossMeasured = -999.;
    }
    
    pTruthMS = 0; //p truth at MS entry
    if (truthMu.isAvailable<float>("MuonEntryLayer_px") &&
	truthMu.isAvailable<float>("MuonEntryLayer_py") &&
	truthMu.isAvailable<float>("MuonEntryLayer_pz") ) {
      TVector3 pvecTruthMS(truthMu.auxdata<float>("MuonEntryLayer_px"),
			   truthMu.auxdata<float>("MuonEntryLayer_py"),
			   truthMu.auxdata<float>("MuonEntryLayer_pz"));
      pTruthMS = pvecTruthMS.Mag();
      etaTruthMS = pvecTruthMS.Eta();
      phiTruthMS = pvecTruthMS.Phi();
    }
    else{
      pTruth = -999;
      etaTruth = -999;
      phiTruth = -999;
    }

    tree->Fill();
  }
   
} // closing namespace Muon
