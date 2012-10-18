/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  cout << "Initializing ROOT fo use with CscSegmentPerformance" << endl;

  gROOT->ProcessLine(".L MuUtil.C");
  gROOT->ProcessLine(".L MuDraw.C");
  gROOT->ProcessLine(".L MuPublish.C");
  gROOT->ProcessLine(".L simseg.C");
  gROOT->ProcessLine(".L segment.C");
  gROOT->ProcessLine(".L segperf.C");
  gROOT->ProcessLine(".L seg4perf.C");
  gROOT->ProcessLine(".L report2d.C");
  gROOT->ProcessLine(".L report4d.C");

  // User may override kinematic cut:
  segperf_cut_kine = "pt>2000";

  segperf_help();
  seg4perf_help();
}
