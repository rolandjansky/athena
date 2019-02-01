/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/CorrectionCode.h>

//
// main program
//

int main ()
{
   CP::CorrectionCode c1( CP::CorrectionCode::Ok );
   CP::CorrectionCode c2( CP::CorrectionCode::OutOfValidityRange );
   CP::CorrectionCode c3( CP::CorrectionCode::Error );

   c3.ignore();

   return 0;
}
