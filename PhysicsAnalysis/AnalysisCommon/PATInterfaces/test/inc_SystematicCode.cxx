/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/SystematicCode.h>

//
// main program
//

int main () {

   CP::SystematicCode c1( CP::SystematicCode::Ok );
   CP::SystematicCode c2( CP::SystematicCode::Unsupported );

   c2.ignore();

   return 0;
}
