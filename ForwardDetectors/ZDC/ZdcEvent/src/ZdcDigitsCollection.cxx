/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "ZdcEvent/ZdcDigitsCollection.h"
#include <iostream>

void ZdcDigitsCollection::print()
{
  std::cout << "Dumping ZdcDigitsCollection" << std::endl;
  int i = 0;
  for (const ZdcDigits* dig : *this)
    {
      std::cout << "Printing Digit #" << i << std::endl;
      dig->print();
      i++;
    }
  return;
}
