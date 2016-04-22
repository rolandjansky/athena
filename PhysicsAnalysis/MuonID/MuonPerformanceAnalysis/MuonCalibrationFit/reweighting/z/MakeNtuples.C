/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void MakeNtuples() {

  TChain* c_data = new TChain( "MuonMomentumCalibrationTree" );
  c_data->Add( "/home/gartoni/CalibFit/fit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/data/Preliminary/Data_Preliminary.root" );
  c_data->Process( "Reweighting.C+", "data.root" );

  TChain* c_mc = new TChain( "MuonMomentumCalibrationTree" );
  c_mc->Add( "/home/gartoni/CalibFit/fit/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonCalibrationFit/data/Preliminary/MonteCarlo_Preliminary.root" );
  c_mc->Process( "Reweighting.C+", "mc.root" );

  gSystem->Exec( "rm rew.root" );
  gSystem->Exec( "hadd rew.root data.root mc.root" );
  gSystem->Exec( "rm data.root mc.root" );

}
