/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgMessaging/StatusCode.h"

int main() {

   // Create some StatusCodes, and check just a few of them:
   StatusCode checkedSuccess( StatusCode::SUCCESS );
   StatusCode checkedFailure( StatusCode::FAILURE );
   StatusCode uncheckedSuccess( StatusCode::SUCCESS );
   StatusCode uncheckedFailure( StatusCode::FAILURE );

   // Some sanity checks:
   if( checkedSuccess.isFailure() ) {
      return 1;
   }
   if( checkedFailure.isSuccess() ) {
      return 1;
   }

   // Return gracefully:
   return 0;
}
