/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/ProofArgs.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/ProofDriver.h>
#include <EventLoop/OutputStream.h>

//
// method implementations
//

ClassImp(EL::ProofArgs)

namespace EL
{
  ProofArgs ::
  ProofArgs ()
    : driver (0)
  {
  }



  ProofArgs ::
  ~ProofArgs ()
  {
    delete driver;
    for (std::vector<Algorithm*>::const_iterator alg = algs.begin(),
	   end = algs.end(); alg != end; ++ alg)
      delete *alg;
  }
}
