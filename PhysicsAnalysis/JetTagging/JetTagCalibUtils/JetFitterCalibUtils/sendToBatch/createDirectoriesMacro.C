/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../preparedatasets/collectionsToProcess.h"
#include <vector>
#include "TSystem.h"

using namespace std;

void createDirectoriesMacro()
{

  gSystem->Exec("mkdir ../trainingResultsJetNet");
  gSystem->Exec("mkdir ../trainingResultsLikelihoodHistos");
  
  std::vector<TString> collections=getCollectionsToProcess();

  for (std::vector<TString>::const_iterator iter=collections.begin();
       iter!=collections.end();iter++)
  {
    
    TString command("mkdir ../trainingResultsJetNet/");
    command+=*iter;
    gSystem->Exec(command);
    TString command2("mkdir ../trainingResultsLikelihoodHistos/");
    command2+=*iter;
    gSystem->Exec(command2);
  }
}


