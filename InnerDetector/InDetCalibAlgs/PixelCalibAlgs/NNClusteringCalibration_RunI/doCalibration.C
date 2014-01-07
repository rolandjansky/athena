/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
gSystem->Load("libTJetNet.so");
gROOT->ProcessLine(".L prepareCalibration.C+");
prepareClusteringCalibrationFile();
}
