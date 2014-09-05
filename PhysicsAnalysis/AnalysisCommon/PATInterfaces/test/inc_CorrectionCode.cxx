//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
