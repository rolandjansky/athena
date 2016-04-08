/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TString.h>

using namespace std;

void writeNtuple_Official(TString inputFileName="all.root",
                          TString jetCollectionName="Cone4H1TowerParticleJets",
                          TString suffix = "AOD",
                          bool forNN = false,
                          bool randomize = false);
  
