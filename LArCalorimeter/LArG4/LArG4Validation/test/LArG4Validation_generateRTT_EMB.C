/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  gSystem->Exec("generate --etaMin=0.2 --etaMax=0.25 --energy=25000 --parameterize=1 --histograms --outputFile=\"generateRTT_EMB\" --skipHits --overrideSeeds --nevents=1000");

}
