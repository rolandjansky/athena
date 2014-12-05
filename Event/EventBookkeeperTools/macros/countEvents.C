/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mark Hodgkinson, University of Sheffield
// Created July 16 2009
// Simple example root macro to extract number of events and weighted events (for e.g. MC@NLO samples )
// To use it simply start ROOT up, after setting up the athena environement in the standard way,
// and type ".x countEvents.C" - note both countEvents.C and test.py have to be in the directory you run ROOT in.

void countEvents(){

  // You might wish to edit test.py to use your pool file, it assumes you have AOD.pool.root...
  TPython::ExecScript("test.py");

  tree = (TTree*)gROOT->FindObjectAny("MetaData_trans");

  const EventBookkeeperCollection* bookCollection;
  TBranch *bookBr = tree->GetBranch("EventBookkeepers");
  bookBr->SetAddress(&bookCollection);
  bookBr->GetEntry(0);
  std::cout << "Number of processed events was " << bookCollection->at(0)->getNAcceptedEvents() << std::endl;
  std::cout << "Number of weighted processed events was " << bookCollection->at(0)->getSumOfWeights() << std::endl;

}
