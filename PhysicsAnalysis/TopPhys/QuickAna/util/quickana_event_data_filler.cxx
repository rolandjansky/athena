/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


/**
  This executable is used by the ut_event_data_filler test.
  It is otherwise likely not useful for users.
*/


//
// includes
//

#include <PATInterfaces/CorrectionCode.h>
#include <PATInterfaces/SystematicsUtil.h>
#include <QuickAna/EventData.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <TFile.h>
#include <TH1.h>
#include <chrono>
#include <xAODRootAccess/Init.h>
#include <CxxUtils/make_unique.h>

//
// main program
//

template<class T,class Store> StatusCode
check (Store& store, const ana::EventData& eventData,
       ana::ObjectType type, const CP::SystematicSet& sys,
       const T *ref, const std::string& sourceName)
{
  using namespace ana::msgUserCode;

  const T *obj = nullptr;
  ANA_CHECK (store.retrieve (obj, eventData.getName (type, sys)));
  if (obj != ref)
  {
    ATH_MSG_ERROR ("retrieved object inconsistent");
    return StatusCode::FAILURE;
  }
  auto source = eventData.getSourceName (type, sys);
  if (sourceName != source)
  {
    ATH_MSG_ERROR ("source object inconsistent: " << sourceName << " " << source);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

int main (int argc, char ** argv)
{
  ANA_CHECK_SET_TYPE (int);

  using namespace ana::msgUserCode;

  // initialize xAOD classes
  ana::enableStatusCodeFailure ();
  ANA_CHECK (xAOD::Init ());

  // command line processing
  if (argc < 2 || argc > 3)
  {
    ATH_MSG_ERROR ("usage: " << argv[0]
		   << " <xAOD.root> [<schedulerDef>]");
    return EXIT_FAILURE;
  }
  const std::string inputFileName = argv[1];
  std::string scheduler = "basic";
  if (argc == 3) {
    scheduler = argv[2];
  }

  // open and attach input xAOD file
  TFile inputFile( inputFileName.c_str() );
  xAOD::TEvent event(xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK( event.readFrom(&inputFile) );

  // create a new QuickAna tool
  auto quickAna = CxxUtils::make_unique<ana::QuickAna>("quickana_" + scheduler);

  // request the actual object definitions to use
  quickAna->eventinfoDef = "default";
  quickAna->muonDef = "default";
  quickAna->electronDef = "default";
  quickAna->photonDef = "default";
  quickAna->tauDef = "default";
  quickAna->jetDef = "default";
  quickAna->metDef = "default";
  quickAna->orDef = "default";

  // We just need some files for testing. Do not use these in your analysis
  quickAna->muMcFiles = {
    "dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
    "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root",
  };
  // Using test file from SUSYTools
  quickAna->muDataFiles = {"dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"};

  // select the proper scheduler
  quickAna->schedulerDef = scheduler;

  // initialize QuickAns tool
  ANA_CHECK (quickAna->initialize());

  ANA_CHECK (quickAna->setSystematics (CP::make_systematics_vector (quickAna->recommendedSystematics())));

  store.clear ();
  event.getEntry (0);

  ana::EventData eventData = quickAna->getEventData();

  for (auto sys : quickAna->systematics())
  {
    // apply systematic variation
    if (quickAna->applySystematicVariation (sys) != CP::SystematicCode::Ok)
      return EXIT_FAILURE;

    // run QuickAna on this event
    ANA_CHECK (quickAna->process (event));

    ANA_CHECK (check (event, eventData, ana::OBJECT_MUON, sys, quickAna->muons(), "Muons"));
    ANA_CHECK (check (event, eventData, ana::OBJECT_ELECTRON, sys, quickAna->electrons(), "Electrons"));
    ANA_CHECK (check (event, eventData, ana::OBJECT_PHOTON, sys, quickAna->photons(), "Photons"));
    ANA_CHECK (check (event, eventData, ana::OBJECT_JET, sys, quickAna->jets(), "AntiKt4EMTopoJets"));
  }

  return EXIT_SUCCESS;
}
