/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/Global.h>

#include <QuickAna/MessageCheck.h>
#include <QuickAna/xAODTruth.h>
#include <RootCoreUtils/Assert.h>
#include <TFile.h>
#include <cstdlib>
#include <iostream>
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>

using namespace ana;

//
// unit test
//

int main ()
{
  ANA_CHECK_SET_TYPE (int);

  // needed to make messaging work
  using namespace ana::msgUserCode;

  // initialize xAOD classes
  ana::enableStatusCodeFailure ();
  ANA_CHECK (xAOD::Init ());

  const char *name = "TruthParticles";
  (void) name;

  const char *fileName = getenv ("ROOTCORE_TEST_FILE");
  if (fileName == nullptr)
  {
    ATH_MSG_ERROR ("no test file defined, exiting");
    return EXIT_SUCCESS;
  }

  std::unique_ptr<TFile> file (TFile::Open (fileName));
  xAOD::TEvent event (xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK (event.readFrom (file.get()));

  event.getEntry (0);
  RCU_ASSERT (getCTruth (event, name) != nullptr);
  RCU_ASSERT (getCTruth (event) != nullptr);
  RCU_ASSERT (getCTruth (name) != nullptr);
  RCU_ASSERT (getCTruth () != nullptr);
  RCU_ASSERT (getCTruth ("UnknownName", true) == nullptr);
  try
  {
    getCTruth ("TruthParticlesDummy");
    RCU_ASSERT0 ("test did not fail");
  } catch (...) {}

  return EXIT_SUCCESS;
}
