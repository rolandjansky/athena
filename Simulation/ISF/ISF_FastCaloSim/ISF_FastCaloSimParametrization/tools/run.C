/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloHitAna.C"
//To be used like this:
//root -b -l -q run.C+\(\"ISF_HitAnalysisttbar.root\"\)
//to produce the ouput file with a simple tree

void run(TString filename="ISF_HitAnalysispi.root")
{
  //CaloHitAna t;
  CaloHitAna t(filename,0);
  t.Loop();


}
