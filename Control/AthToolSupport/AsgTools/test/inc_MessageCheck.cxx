/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/MessageCheck.h>

using namespace asg;

//
// main program
//

int main ()
{
  using namespace msgUserCode;

  ANA_CHECK (StatusCode ());
  ANA_CHECK_THROW (StatusCode ());
  ANA_MSG_ERROR ("message");
  return 0;
}
