/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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
