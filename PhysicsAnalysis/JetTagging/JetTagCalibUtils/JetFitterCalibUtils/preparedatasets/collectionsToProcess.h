/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <TString.h>

using namespace std;

vector<TString> getCollectionsToProcess() 
{
  
  vector<TString> collectionsToProcess;
  
  //collectionsToProcess.push_back("Cone4H1TowerParticleJets");
  //collectionsToProcess.push_back("Cone7H1TowerParticleJets");
  //collectionsToProcess.push_back("Kt6H1TowerParticleJets");
  //collectionsToProcess.push_back("Kt4H1TowerParticleJets");

  collectionsToProcess.push_back("Cone4H1TopoJets");
  collectionsToProcess.push_back("Cone7LCTopoJets");
  collectionsToProcess.push_back("Kt4LCTopoJets");
  collectionsToProcess.push_back("Kt6LCTopoJets");

  collectionsToProcess.push_back("Cone4H1TowerJets");
 collectionsToProcess.push_back("Cone7H1TowerJets");

  collectionsToProcess.push_back("Kt4TruthJets");
  collectionsToProcess.push_back("Kt6TruthJets");
  collectionsToProcess.push_back("Cone7TruthJets");
  collectionsToProcess.push_back("Cone4TruthJets");

  
  return collectionsToProcess;
  
}

vector<TString> getAllCollections() 
{
  
  vector<TString> collectionsToProcess;
  
  collectionsToProcess.push_back("Cone4H1TowerParticleJets");
  collectionsToProcess.push_back("Cone7H1TowerParticleJets");
  collectionsToProcess.push_back("Kt6H1TowerParticleJets");
  collectionsToProcess.push_back("Kt4H1TowerParticleJets");

  collectionsToProcess.push_back("Cone4H1TopoParticleJets");
  collectionsToProcess.push_back("Cone7H1TopoParticleJets");
  collectionsToProcess.push_back("Kt6H1TopoParticleJets");
  collectionsToProcess.push_back("Kt4H1TopoParticleJets");

  collectionsToProcess.push_back("Cone4TruthParticleJets");
  collectionsToProcess.push_back("Cone7TruthParticleJets");
  collectionsToProcess.push_back("Kt6TruthParticleJets");
  collectionsToProcess.push_back("Kt4TruthParticleJets");
  

//  collectionsToProcess.push_back("Cone4H1TowerParticleJets");

  return collectionsToProcess;
  
}
