/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  gSystem->Exec("generate --etaMin=3.9 --etaMax=4.1 --energy=50000 --parameterize=1 --histograms --outputFile=\"generateRTT_FCAL\" --skipHits --overrideSeeds --nevents=1000");

}
