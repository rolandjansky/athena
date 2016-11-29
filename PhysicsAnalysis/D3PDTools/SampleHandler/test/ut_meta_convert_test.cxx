/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <SampleHandler/MetaObject.h>
#include <iostream>

//
// main program
//

using namespace SH;

int main ()
{

  try
  {
    MetaObject* meta = new MetaObject();
    meta->setString("the_answer_to_life_the_universe_and_everything","42");
    std::cout << meta->castDouble("the_answer_to_life_the_universe_and_everything",0) << std::endl;

  } catch (std::exception& e)
  {
    std::cout << "caught exception: " << e.what() << std::endl;
    return 1;
  }
}
