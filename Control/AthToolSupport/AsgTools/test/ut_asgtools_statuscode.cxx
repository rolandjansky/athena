// $Id: ut_asgtools_statuscode.cxx 612639 2014-08-20 12:26:10Z krasznaa $

// Local include(s):
#include "AsgTools/StatusCode.h"

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
