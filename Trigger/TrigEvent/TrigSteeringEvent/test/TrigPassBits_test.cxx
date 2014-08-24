/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <iostream>
#include "TrigSteeringEvent/TrigPassBits.h"

class Feature {};

int main() {


  Feature* a1 = new Feature; 
  Feature* a2 = new Feature; 
  Feature* a3 = new Feature; 
  //Feature* a4 = new Feature; 
  std::vector<Feature*> container;
  container.push_back(a1);
  container.push_back(a2);
  container.push_back(a3);

  //TrigPassBits *bits = new TrigPassBits(container.size());
  TrigPassBits *bits = HLT::makeTrigPassBits(&container);
  std::cout << " size " << bits->size() << "==" <<container.size() <<  std::endl;
  HLT::markPassing(bits, a1, &container);
  HLT::markPassing(bits, a3, &container);

  //mark_passing(bits, a4, &container);  // thows an exception

  std::cout << HLT::isPassing(bits, a1, &container) 
	    << " " << HLT::isPassing(bits, a2, &container) 
	    << " " << HLT::isPassing(bits, a3, &container) << std::endl;

  
  //is_passing(bits, a4, &container);

}

