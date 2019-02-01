/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/MessageCheck.h>

using namespace ana;

//
// main program
//

int main ()
{
  using namespace msgToolHandle;

  ANA_CHECK (StatusCode ());
  ANA_MSG_ERROR ("message");
  return 0;
}
