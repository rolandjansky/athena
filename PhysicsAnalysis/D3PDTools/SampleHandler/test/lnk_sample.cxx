/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/Global.h>

#include <SampleHandler/SampleComposite.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleHist.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SampleMeta.h>

//
// main program
//

using namespace SH;

int main ()
{
  new SampleComposite;
  new SampleGrid;
  new SampleHist;
  new SampleLocal;
  new SampleMeta;

  return 0;
}
