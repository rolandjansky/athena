/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonHistUtils/SlowMuonParamPlots.h"
#include "TMath.h" // for TMath::Prob()

namespace Muon{

  BetaPlots::BetaPlots(PlotBase *pParent, const std::string& sDir, std::string sDetType)
  :PlotBase(pParent, sDir)
  , nHits(nullptr)
  , avg(nullptr)
  , chi2(nullptr)
  , ndf(nullptr)
  , chi2ndf(nullptr)
  , chi2prob(nullptr)
  , rms(nullptr)
  , m_sDetType(std::move(sDetType))
{}
void BetaPlots::initializePlots()
{
  nHits = Book1D( m_sDetType+"nHits", "; hits;Entries",20,0,20);
  avg = Book1D( m_sDetType+"avg", "; avg;Entries",20,0,20);
  chi2 = Book1D( m_sDetType+"chi2", "; chi2;Entries",20,0,20);
  ndf = Book1D( m_sDetType+"ndf", "; ndf;Entries",20,0,20);
  chi2ndf = Book1D( m_sDetType+"chi2ndf", "; chi2/ndf;Entries",20,0,5);
  chi2prob = Book1D( m_sDetType+"chi2prob", "; chi2 prob;Entries",20,0,1);
  rms = Book1D( m_sDetType+"rms", "; rms;Entries",20,0,20);  
}

  void BetaPlots::fill(const xAOD::SlowMuon& smu, float weight)
{
  if (m_sDetType=="rpc" || m_sDetType=="Rpc" || m_sDetType=="RPC") {
    nHits->Fill( smu.nRpcHits(), weight );
    ndf->Fill( smu.rpcBetaDof(), weight );
    if (smu.rpcBetaDof()>0) {
      avg->Fill( smu.rpcBetaAvg(), weight );
      rms->Fill( smu.rpcBetaRms(), weight );
      chi2->Fill( smu.rpcBetaChi2(), weight );
      ndf->Fill( smu.rpcBetaDof(), weight );
      chi2ndf->Fill( smu.rpcBetaChi2()/smu.rpcBetaDof(), weight );
      chi2prob->Fill( TMath::Prob(smu.rpcBetaChi2(), smu.rpcBetaDof()), weight );
    }
  }
  else if (m_sDetType=="mdt" || m_sDetType=="Mdt" || m_sDetType=="MDT") {
    //nHits->Fill( smu.nMdtHits() );
    ndf->Fill( smu.mdtBetaDof(), weight );
    if (smu.mdtBetaDof()>0) {
      avg->Fill( smu.mdtBetaAvg(), weight );
      rms->Fill( smu.mdtBetaRms(), weight );
      chi2->Fill( smu.mdtBetaChi2(), weight );
      ndf->Fill( smu.mdtBetaDof(), weight );	    
      chi2ndf->Fill( smu.mdtBetaChi2()/smu.mdtBetaDof(), weight );
      chi2prob->Fill( TMath::Prob(smu.mdtBetaChi2(), smu.mdtBetaDof()), weight );
    }
  }
  else if (m_sDetType=="calo" || m_sDetType=="Calo" || m_sDetType=="CALO") {
    nHits->Fill( smu.nTileCells(), weight );
    ndf->Fill( smu.caloBetaDof(), weight );
    if (smu.caloBetaDof()>0) {
      avg->Fill( smu.caloBetaAvg(), weight );
      rms->Fill( smu.caloBetaRms(), weight );
      chi2->Fill( smu.caloBetaChi2(), weight );
      ndf->Fill( smu.caloBetaDof(), weight );
      chi2ndf->Fill( smu.caloBetaChi2()/smu.caloBetaDof(),weight );
      chi2prob->Fill( TMath::Prob(smu.caloBetaChi2(), smu.caloBetaDof()), weight );
    }
  }

}

/////////////////////////////////
  
SlowMuonParamPlots::SlowMuonParamPlots(PlotBase *pParent, const std::string& sDir)
  :PlotBase(pParent, sDir)
  , beta(nullptr)
  , mdtBeta(this,"","MDT")
  , rpcBeta(this,"","RPC")
  , caloBeta(this,"","CALO")
{}

void SlowMuonParamPlots::initializePlots()
{
  beta = Book1D("beta", "beta;beta;Entries",150,0,1.5);  
}

  void SlowMuonParamPlots::fill(const xAOD::SlowMuon& smu, float weight)
{
  beta->Fill( smu.beta(), weight );
  mdtBeta.fill( smu , weight);
  rpcBeta.fill( smu, weight );
  caloBeta.fill( smu, weight );
  
}  

} //close namespace Muon
