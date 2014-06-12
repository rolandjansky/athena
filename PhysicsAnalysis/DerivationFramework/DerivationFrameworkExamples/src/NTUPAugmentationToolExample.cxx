/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NTUPAugmentationToolExample.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// Based on the Integrated Simulation Framework
//
// This is a trivial example of tool that creates extra information
// and places it in the StoreGate

#include "DerivationFrameworkExamples/NTUPAugmentationToolExample.h"

#include <vector>
#include <string>

namespace DerivationFramework {

  NTUPAugmentationToolExample::NTUPAugmentationToolExample(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode NTUPAugmentationToolExample::addBranches() const
  {
    int *someint = new int;
    std::vector<int> *someintV = new std::vector<int>();
    std::vector<std::vector<int> > *someintVV = new std::vector<std::vector<int> >();
    *someint = 42;
    someintV->push_back(1); someintV->push_back(3);
    std::vector<int> test;
    test.push_back(5); test.push_back(10);
    std::vector<int> test2;
    test2.push_back(5); test2.push_back(10);
    someintVV->push_back(test2);
    CHECK(evtStore()->record(someint, "tpgTestI"));
    CHECK(evtStore()->record(someintV, "tpgTestVI"));
    CHECK(evtStore()->record(someintVV, "tpgTestVVI"));

    return StatusCode::SUCCESS;
  }
}
