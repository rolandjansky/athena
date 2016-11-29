/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void retrieve (const char *location)
{
  // do the actual retrieve
  if (EL::Driver::retrieve (location))
    std::cout << "retrieved output completely" << std::endl;
  else
    std::cout << "output incomplete" << std::endl;
}
