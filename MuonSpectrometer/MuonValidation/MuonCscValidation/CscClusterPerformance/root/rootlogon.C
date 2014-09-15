/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  gROOT->ProcessLine(".X loadMuonRootUtils.C");
  gROOT->ProcessLine(".L atlas_style.C"); // added by Kyoko 2009-12-15
  gROOT->ProcessLine(".L CscChamberPlot.C" );
  gROOT->ProcessLine(".L CscTupleSequencer.C" );
  gROOT->ProcessLine(".L CluperfViewer.C" );
  gROOT->ProcessLine(".L simpos.C");
  gROOT->ProcessLine(".L digit.C");
  gROOT->ProcessLine(".L cluster.C");
  gROOT->ProcessLine(".L cscperf.C");
  gROOT->ProcessLine(".L report_perf.C");
  cscperf_help();

  gROOT->SetStyle("Plain"); // white color for margin
  gStyle->SetTitleFillColor(0);

  atlas_style();
  
  // User may override default cuts here.
  cluperf_cut_kine = "pt>2000";

  // Configure MuDraw to use the cluster performance tuple.
  MuDraw::default_treename() = "csc_perf";
 
  // Define drawing objects.

  MuDraw cpeta("cpeta", "Eta cluster: truth, found and lost");
  cpeta.set_cut(cluperf_cut_sim_eta);
  cpeta.append("truth", "", 38, 3003);
  cpeta.append("found", cluperf_cut_found_eta, 30, 3001);
  cpeta.append("lost", cluperf_cut_lost_eta, 46, 3010);

  MuDraw cpphi("cpphi", "Phi cluster: truth, found and lost");
  cpphi.set_cut(cluperf_cut_sim_phi);
  cpphi.append("truth", "", 38, 3003);
  cpphi.append("found", cluperf_cut_found_phi, 30, 3001);
  cpphi.append("lost", cluperf_cut_lost_phi, 46, 3010);

  MuDraw cfeta("cfeta", "Found eta cluster: unspoiled and spoiled");
  cfeta.set_cut(cluperf_cut_found_eta);
  cfeta.append("found", "", 16, 3003);
  cfeta.append("unspoiled", "(szc==0 || szc==10)", 30, 3001);
  cfeta.append("spoiled", "(szc!=0 && szc!=10)", 46, 3010);

  MuDraw cfphi("cfphi", "Found phi cluster");
  cfphi.set_cut(cluperf_cut_found_phi);
  cfphi.append("found", "", 38, 3003);

  MuDraw cfueta("cfueta", "Found unspoiled eta cluster");
  cfueta.set_cut(cluperf_cut_found_eta + "pt>2000");
  cfueta.append("unspoiled", "(szc==0 || szc==10)", 30, 3001);

  MuDraw cfueta_css("cfueta_css", "Found unspoiled eta cluster CSS");
  cfueta_css.set_cut(cluperf_cut_found_eta + "pt>2000 && istation==1");
  cfueta_css.append("unspoiled", "(szc==0 || szc==10)", 30, 3001);

  MuDraw cfueta_csl("cfueta_csl", "Found unspoiled eta cluster CSL");
  cfueta_csl.set_cut(cluperf_cut_found_eta + "pt>2000 && istation==2");
  cfueta_csl.append("unspoiled", "(szc==0 || szc==10)", 30, 3001);


}
