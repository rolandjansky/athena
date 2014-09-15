/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  TFile::Open("/usatlas/u/dladams/muperf/csc/tuples/calib0.005145.PythiaZmumu.digit.RDO.v12003101/threshold/csc_perf.root");
  EventNumberEvaluator<CluperfReader> ev(cluperf_reader);
  CscTupleSequencer seq(&ev);
  seq.sequence(cluperf());
  cout << "Number of entries is " << seq.size() << endl;
  int last = seq.size() - 1;
  cout << "Smallest event (entry " << seq.entry(0)    << ") is " << seq.value(0)    << endl;
  cout << "Biggest event (entry  " << seq.entry(last) << ") is " << seq.value(last) << endl;
}
