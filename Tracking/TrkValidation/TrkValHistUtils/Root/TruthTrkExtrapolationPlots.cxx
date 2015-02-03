/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

namespace Trk{

void TruthTrkExtrapolationPlots::init(){
    CaloEntry_p=NULL;
    MuonEntry_p=NULL;
    MuonExit_p=NULL;
    CaloEntry_pExtr=NULL;
    MuonEntry_pExtr=NULL;
    MuonExit_pExtr=NULL;
    CaloEntry_dp=NULL;
    MuonEntry_dp=NULL;
    MuonExit_dp=NULL;
}

void TruthTrkExtrapolationPlots::initializePlots(){
  CaloEntry_p = Book1D("CaloEntry_p","CaloEntry_p;p_{nom} @CaloEntry;Entries",100,0,400);
  MuonEntry_p = Book1D("MuonEntry_p","MuonEntry_p;p_{nom} @MuonEntry;Entries",100,0,400);
  MuonExit_p  = Book1D("MuonExit_p","MuonExit_p;p_{nom} @MuonExit;Entries",100,0,400);
  CaloEntry_pExtr = Book1D("CaloEntry_pExtr","CaloEntry_p;p_{ext} @CaloEntry;Entries",100,0,400);
  MuonEntry_pExtr = Book1D("MuonEntry_pExtr","MuonEntry_p;p_{ext} @MuonEntry;Entries",100,0,400);
  MuonExit_pExtr  = Book1D("MuonExit_pExtr","MuonExit_p;p_p_{ext} @MuonExit;Entries",100,0,400);;
  CaloEntry_dp = Book1D("CaloEntry_dp","CaloEntry_dp;p_{nom}- p_{ext} @CaloEntry;Entries",100,-10,10);
  MuonEntry_dp = Book1D("MuonEntry_dp","MuonEntry_dp;p_{nom}- p_{ext} @MuonEntry;Entries",100,-10,10);
  MuonExit_dp  = Book1D("MuonExit_dp","MuonExit_dp;p_{nom}- p_{ext} @MuonExit;Entries",100,-10,10);

}

void TruthTrkExtrapolationPlots::fill(const xAOD::TruthParticle& truthprt) {
  FillPlots(CaloEntry_dp, truthprt, "CaloEntryLayer");
  FillPlots(MuonEntry_dp, truthprt, "MuonEntryLayer");
  FillPlots(MuonExit_dp,  truthprt, "MuonExitLayer");  
}


void TruthTrkExtrapolationPlots::FillPlots(TH1* hist, const xAOD::TruthParticle& truthprt,const std::string& sNom ) {
  if (!truthprt.isAvailable<float>(sNom + "_px") || 
      !truthprt.isAvailable<float>(sNom + "_py") || 
      !truthprt.isAvailable<float>(sNom + "_pz") || 
      !truthprt.isAvailable<float>(sNom + "_px_extr" ) ||
      !truthprt.isAvailable<float>(sNom + "_py_extr" ) ||
      !truthprt.isAvailable<float>(sNom + "_pz_extr" )) return;

  TVector3 nomVec(truthprt.auxdata<float>(sNom + "_px"), 
                  truthprt.auxdata<float>(sNom + "_py"), 
                  truthprt.auxdata<float>(sNom + "_pz"));

  TVector3 exVec(truthprt.auxdata<float>(sNom + "_px_extr"), 
                 truthprt.auxdata<float>(sNom + "_py_extr"), 
                 truthprt.auxdata<float>(sNom + "_pz_extr"));

  float pNom = nomVec.Mag();
  float pExt = exVec.Mag();
  hist->Fill((pNom-pExt)*0.001);

  if (sNom=="CaloEntryLayer") {
    CaloEntry_p->Fill(pNom*0.001);
    CaloEntry_pExtr->Fill(pExt*0.001);
  }
  else if (sNom=="MuonEntryLayer") {
    MuonEntry_p->Fill(pNom*0.001);
    MuonEntry_pExtr->Fill(pExt*0.001);
  }
  else if (sNom=="MuonExitLayer") {
    MuonExit_p->Fill(pNom*0.001);
    MuonExit_pExtr->Fill(pExt*0.001);
  }

}

}
