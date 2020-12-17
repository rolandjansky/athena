/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgMessaging/StatusCode.h>

//
// main program
//

int main () {

   StatusCode c1( StatusCode::SUCCESS );
   StatusCode c2( StatusCode::FAILURE );

   c2.ignore();

   return 0;
}
