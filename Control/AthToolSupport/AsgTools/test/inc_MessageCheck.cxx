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
