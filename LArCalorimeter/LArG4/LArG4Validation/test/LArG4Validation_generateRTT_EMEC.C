/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{

  gSystem->Exec("generate --etaMin=2.0 --etaMax=2.1 --energy=25000 --parameterize=1 --histograms --outputFile=\"generateRTT_EMEC\" --skipHits --overrideSeeds --nevents=1000");

}
