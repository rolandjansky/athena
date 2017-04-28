/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void submit (const char *submitDir)
{
  TString input = "$ROOTCOREBIN/data/EventLoop";
  gSystem->ExpandPathName (input);

  SH::SampleHandler sh;
  SH::ScanDir().filePattern ("test_ntuple*.root").scan (sh, input.Data());

  EL::Job job;
  job.sampleHandler (sh);

  EL::DirectDriver().submitOnly (job, submitDir);
}
