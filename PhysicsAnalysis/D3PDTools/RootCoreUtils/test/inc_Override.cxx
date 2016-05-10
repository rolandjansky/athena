//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/Override.h>

using namespace RCU;

//
// main program
//

struct A
{
  virtual void f () = 0;
};

struct B : public A
{
  virtual void f () RCU_OVERRIDE;
};

void B :: f () {}

int main ()
{
  return 0;
}
