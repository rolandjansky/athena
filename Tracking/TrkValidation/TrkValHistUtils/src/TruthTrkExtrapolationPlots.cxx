/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

namespace Trk{

void TruthTrkExtrapolationPlots::initializePlots(){
  CaloEntry_p = Book1D("CaloEntry_p","CaloEntry_p;p_{nom}- p_{ext} @CaloEntry;Entries",100,-10,10);
  MuonEntry_p = Book1D("MuonEntry_p","MuonEntry_p;p_{nom}- p_{ext} @MuonEntry;Entries",100,-10,10);
  MuonExit_p  = Book1D("MuonExit_p","MuonExit_p;p_{nom}- p_{ext} @MuonExit;Entries",100,-10,10);
}

void TruthTrkExtrapolationPlots::fill(const xAOD::TruthParticle& truthprt) {
  FillPlots(CaloEntry_p, truthprt, "CaloEntryLayer");
  FillPlots(MuonEntry_p, truthprt, "MuonEntryLayer");
  FillPlots(MuonExit_p,  truthprt, "MuonExitLayer");  
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

  hist->Fill((nomVec.Mag() - exVec.Mag())*0.001);  
}

}
