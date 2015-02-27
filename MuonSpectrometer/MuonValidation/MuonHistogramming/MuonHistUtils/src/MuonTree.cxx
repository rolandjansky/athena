/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonTree.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "TROOT.h"

namespace Muon{

  MuonTree::MuonTree(PlotBase* pParent, std::string sDir):
    PlotBase(pParent, sDir),
    tree(NULL)
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

  void MuonTree::fillTree(const xAOD::Muon& muon, const xAOD::TrackParticle* muonTrackIP, const xAOD::TruthParticle& truthMu)
  {
    const xAOD::TrackParticle* muPrimaryTrk = muon.trackParticle(xAOD::Muon::Primary);
    if (!muPrimaryTrk) return;
    const xAOD::TrackParticle* msTrk = muon.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);//muon extrapolated to IP
    std::cout<<"Calling the fillTree of Mini Tree class"<<std::endl; 
    pTruth = truthMu.p4().P();
    pCB = muPrimaryTrk->p4().P();

    etaTruth = truthMu.p4().Eta();
    etaCB = muPrimaryTrk->p4().Eta();

    phiTruth = truthMu.p4().Phi();
    phiCB = muPrimaryTrk->p4().Phi();


    pMExtrapol = 0;
    if (msTrk) {
      pMExtrapol = msTrk->p4().P();
      etaMExtrapol = msTrk->p4().Eta();
      phiMExtrapol = msTrk->p4().Phi();
    }
    else{
      pMExtrapol = -999;
      etaMExtrapol = -999;
      phiMExtrapol = -999;
    }
    //muon extrapolated
    pMS = 0;
    if (muonTrackIP) {
      pMS = muonTrackIP->p4().P(); //at muon spectrometer entry//@@@
      etaMS = muonTrackIP->p4().Eta(); //at muon spectrometer entry//@@@
      phiMS = muonTrackIP->p4().Phi(); //at muon spectrometer entry//@@@
    }
    else{
      pMS = -999; //at muon spectrometer entry//@@@
      etaMS = -999; //at muon spectrometer entry//@@@
      phiMS = -999; //at muon spectrometer entry//@@@
    }
    elossMeasured = 0;
    muon.parameter(elossMeasured,xAOD::Muon::MeasEnergyLoss);
    
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
