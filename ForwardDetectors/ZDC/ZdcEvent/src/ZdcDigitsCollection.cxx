/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ZdcEvent/ZdcDigitsCollection.h"
#include <iostream>

void ZdcDigitsCollection::print()
{
  iterator it = this->begin();
  iterator it_end = this->end();

  std::cout << "Dumping ZdcDigitsCollection" << std::endl;
  int i = 0;
  while (it != it_end)
    {
      std::cout << "Printing Digit #" << i << std::endl;
      (*it)->print();
      //std::cout << (*it) << std::endl;
      it++;
      i++;
    }
  return;
}
