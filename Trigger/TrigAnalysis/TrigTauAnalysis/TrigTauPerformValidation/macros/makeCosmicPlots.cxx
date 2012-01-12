/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

makeCosmicPlots() {

  // Could be either DEFAULT - normal trigger progression
  // or RECO_BASED - starts with offline tau
  TTPValCosmicPlots tp(TTPValCosmicPlots::RECO_BASED);

  tp.clear();

  // Define some menu selection
  tp.setDefaultVersion(TrigTauSelectionManager::MENU1560);
  tp.setSelector(TrigTauFlags::TAUNOCUT);

  // Data sample, here reading from CAF (only from lxplus)
  // For MC data replace "Tau/TrigTau" with "EventBranch"
  TChain* chain = new TChain("Tau/TrigTau");

  // Can add as many directories here as desired
  chain->Add("root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/tau/TTPCosmic09/data09_cos.00138065.physics_L1Calo.merge.NTUP_TRIG.f163_m226/*");

  tp.fill(chain);

  // Standard plots
  tp.draw("r138065_reco.ps");

  // Efficiency plots
  //tp.drawEff("r138065_eff_reco.ps");

}
