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
