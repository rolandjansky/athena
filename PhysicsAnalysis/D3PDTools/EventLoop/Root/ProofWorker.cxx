/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/ProofWorker.h>

#include <TChain.h>
#include <TChainElement.h>
#include <TFile.h>
#include <EventLoop/Algorithm.h>
#include <EventLoop/Job.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/Sample.h>

//
// method implementations
//

namespace EL
{
  void ProofWorker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  ProofWorker ::
  ProofWorker (const SH::MetaObject *val_metaData, TList *output,
	       ProofTSelector *selector)
    : Worker (val_metaData, output), m_selector (selector)
  {
    RCU_NEW_INVARIANT (this);
  }
}
