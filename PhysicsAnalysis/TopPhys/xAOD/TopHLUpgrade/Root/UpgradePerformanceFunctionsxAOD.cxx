/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopHLUpgrade/UpgradePerformanceFunctionsxAOD.h"

UpgradePerformanceFunctionsxAOD::UpgradePerformanceFunctionsxAOD() :
  UpgradePerformanceFunctions(),
  m_randgen(811) //fix random seed
{

}

UpgradePerformanceFunctionsxAOD::UpgradePerformanceFunctionsxAOD(UpgradeLayout layout, double avgMu) :
  UpgradePerformanceFunctions(layout, avgMu),
  m_randgen(811) // fix random seed
{

}

/**
 * Get smeared electron energy from the UpgradePerformanceFunctions and then
 * modify the input electron energy accordingly.
 */
void UpgradePerformanceFunctionsxAOD::smearElectron(xAOD::TruthParticle& electron) {
  const double newE = getElectronSmearedEnergy( electron.e(), electron.eta() );
  const double newE2 = newE*newE;
  const double m2 = electron.m() * electron.m();
  const double p2 = newE2 > m2 ? newE2 - m2 : 0.;
  TLorentzVector newvec;
  newvec.SetPtEtaPhiE( sqrt(p2) / cosh(electron.eta()), electron.eta(), electron.phi(), newE );
  
  // no p4 setter in truth particle, have to set one-by-one
  electron.setPx( newvec.Px() );
  electron.setPy( newvec.Py() );
  electron.setPz( newvec.Pz() );
  electron.setE( newvec.E() );
  electron.setM( newvec.M() );
}

/**
 * Get smeared muon momentum from the UpgradePerformanceFunctions and then
 * modify the input muon accordingly. Note, the PdgId will be changed
 * if the muon smearing results in a charge flip (since the 
 * TruthParticle->charge() function uses the PdgId).
 */
void UpgradePerformanceFunctionsxAOD::smearMuon(xAOD::TruthParticle& muon) {
  double qoverpt = muon.charge()/muon.pt();
  const double sigma_qoverpt  =   this->getMuonQOverPtResolution(muon.pt(),  muon.eta());
  qoverpt +=   m_randgen.Gaus(0,sigma_qoverpt);
  const double pt_smeared = fabs(1.0/qoverpt);
  const double q_smeared = (qoverpt > 0) ? 1.0 : -1.0;
  TLorentzVector newvec;
  newvec.SetPtEtaPhiM( pt_smeared, muon.eta(), muon.phi(), muon.m() );
  
  // no p4 setter in truth particle, have to set one-by-one
  muon.setPx( newvec.Px() );
  muon.setPy( newvec.Py() );
  muon.setPz( newvec.Pz() );
  muon.setE( newvec.E() );
  muon.setM( newvec.M() );
  // set pdg id (which is used to get charge)
  muon.setPdgId( -1.0 * q_smeared * muon.absPdgId());
}

/**
 * Get smeared jet pT from the UpgradePerformanceFunctions and then
 * modify the input jet accordingly.
 */
void UpgradePerformanceFunctionsxAOD::smearJet(xAOD::Jet& jet) {
  // can only smear jets with 10 < pT < 1500 GeV (would be good not to hard code this....)
  if(jet.pt() < 10.0*1000.0 or jet.pt() > 1500.0*1000.0) return;

  // function might be called 'SmearedEnergy', but apparently it gives smeared pT (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/UpgradePerformanceFunctions)
  const double smearedjetpt = this->getJetSmearedEnergy(jet.pt(), jet.eta(), true);
  const double smearedjetE = jet.e() * smearedjetpt/jet.pt();
  // lazy way to calculate the mass after pT smearing
  TLorentzVector newjet;
  newjet.SetPtEtaPhiE( smearedjetpt, jet.eta(), jet.phi(), smearedjetE );
  // jet 4 momentum stored as PtEtaPhiM (see xAODJet/JetTypes.h)
  xAOD::JetFourMom_t mom( smearedjetpt, jet.eta(), jet.phi(), newjet.M() );
  jet.setJetP4( mom );
}
