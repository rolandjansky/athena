/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

gSystem->Load("../jetnetRoot/libTJetNet.so");
gSystem->Load("prepareCalibrationFileForCOOL_C.so");
prepareCalibrationFileForCOOL(true);

}

