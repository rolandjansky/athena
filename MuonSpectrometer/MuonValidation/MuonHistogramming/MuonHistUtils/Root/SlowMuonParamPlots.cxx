/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/SlowMuonParamPlots.h"
#include "TMath.h"

namespace Muon{

  BetaPlots::BetaPlots(PlotBase *pParent, std::string sDir, std::string sDetType)
  :PlotBase(pParent, sDir)
  , nHits(NULL)
  , avg(NULL)
  , chi2(NULL)
  , ndf(NULL)
  , chi2ndf(NULL)
  , chi2prob(NULL)
  , rms(NULL)
  , m_sDetType(sDetType)
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

void BetaPlots::fill(const xAOD::SlowMuon& smu)
{
  if (m_sDetType=="rpc" || m_sDetType=="Rpc" || m_sDetType=="RPC") {
    nHits->Fill( smu.nRpcHits() );
    ndf->Fill( smu.rpcBetaDof() );
    if (smu.rpcBetaDof()>0) {
      avg->Fill( smu.rpcBetaAvg() );
      rms->Fill( smu.rpcBetaRms() );
      chi2->Fill( smu.rpcBetaChi2() );
      ndf->Fill( smu.rpcBetaDof() );
      chi2ndf->Fill( smu.rpcBetaChi2()/smu.rpcBetaDof() );
      chi2prob->Fill( TMath::Prob(smu.rpcBetaChi2(), smu.rpcBetaDof()) );
    }
  }
  else if (m_sDetType=="mdt" || m_sDetType=="Mdt" || m_sDetType=="MDT") {
    //nHits->Fill( smu.nMdtHits() );
    ndf->Fill( smu.mdtBetaDof() );
    if (smu.mdtBetaDof()>0) {
      avg->Fill( smu.mdtBetaAvg() );
      rms->Fill( smu.mdtBetaRms() );
      chi2->Fill( smu.mdtBetaChi2() );
      ndf->Fill( smu.mdtBetaDof() );	    
      chi2ndf->Fill( smu.mdtBetaChi2()/smu.mdtBetaDof() );
      chi2prob->Fill( TMath::Prob(smu.mdtBetaChi2(), smu.mdtBetaDof()) );
    }
  }
  else if (m_sDetType=="calo" || m_sDetType=="Calo" || m_sDetType=="CALO") {
    nHits->Fill( smu.nTileCells() );
    ndf->Fill( smu.caloBetaDof() );
    if (smu.caloBetaDof()>0) {
      avg->Fill( smu.caloBetaAvg() );
      rms->Fill( smu.caloBetaRms() );
      chi2->Fill( smu.caloBetaChi2() );
      ndf->Fill( smu.caloBetaDof() );
      chi2ndf->Fill( smu.caloBetaChi2()/smu.caloBetaDof() );
      chi2prob->Fill( TMath::Prob(smu.caloBetaChi2(), smu.caloBetaDof()) );
    }
  }

}

/////////////////////////////////
  
SlowMuonParamPlots::SlowMuonParamPlots(PlotBase *pParent, std::string sDir)
  :PlotBase(pParent, sDir)
  , beta(NULL)
  , mdtBeta(this,"","MDT")
  , rpcBeta(this,"","RPC")
  , caloBeta(this,"","CALO")
{}

void SlowMuonParamPlots::initializePlots()
{
  beta = Book1D("beta", "beta;beta;Entries",150,0,1.5);  
}

void SlowMuonParamPlots::fill(const xAOD::SlowMuon& smu)
{
  beta->Fill( smu.beta() );
  mdtBeta.fill( smu );
  rpcBeta.fill( smu );
  caloBeta.fill( smu );
  
}  

} //close namespace Muon
