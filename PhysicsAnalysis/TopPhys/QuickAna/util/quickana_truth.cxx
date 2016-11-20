/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//
// includes
//

#include <PATInterfaces/CorrectionCode.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/QuickAna.h>
#include <TFile.h>
#include <chrono>
#include <xAODRootAccess/Init.h>

//
// main program
//

int main (int argc, char **argv)
{
  // declare the return type of this function for ANA_CHECK
  ANA_CHECK_SET_TYPE (int);

  // needed for messaging macros
  using namespace ana::msgUserCode;

  // make unchecked status codes abort the job
  ana::enableStatusCodeFailure ();

  // initialize xAOD classes
  ANA_CHECK (xAOD::Init ());


  // command line processing
  if (argc != 2)
  {
    ATH_MSG_ERROR ("usage: " << argv[0] << " <xAOD.root>");
    return EXIT_FAILURE;
  }
  const std::string input_file = argv[1];

  // open and attach input xAOD file
  TFile file(input_file.c_str());
  xAOD::TEvent event (xAOD::TEvent::kAthenaAccess);
  xAOD::TStore store;
  ANA_CHECK (event.readFrom(&file));

  // create a new QuickAna tool
  ana::QuickAna *quickAna = new ana::QuickAna ("quickana");

  // request the actual object definitions to use
  quickAna->eventinfoDef = "default";
  quickAna->jetDef = "truth";
  quickAna->jetKine = "pt > 50e3 && eta < 2.4 && eta > -2.4";

  // initialize QuickAna tool
  ANA_CHECK (quickAna->initialize());

  Long64_t entries = std::min(event.getEntries(), 500ll);
  if (entries > 100)
    entries = 100;
  ATH_MSG_INFO ("Processing " << entries << " entries");

  const auto startTime = std::chrono::steady_clock::now();
  auto lastPrintoutTime = startTime;
  for (Long64_t entry = 0; entry < entries; ++ entry)
  {
    const auto currentStartTime = std::chrono::steady_clock::now();
    if (currentStartTime - lastPrintoutTime > std::chrono::seconds (5))
    {
      lastPrintoutTime = currentStartTime;
      float rate = entry;
      rate /= (lastPrintoutTime - startTime) / std::chrono::seconds(1);
      ATH_MSG_INFO ("Processing entry " << entry << " " << rate << "evt/s");
    }

    store.clear ();
    event.getEntry (entry);

    // run QuickAna on this event
    ANA_CHECK (quickAna->process (event));

    // loop over jets and print
    for (auto jet : *quickAna->jets())
    {
      if (jet->auxdata<ana::SelectType> ("ana_select"))
      {
        ATH_MSG_INFO ("Jet: pt " << jet->pt()*0.001 << " eta " << jet->eta());
      }
    }

  }

  ATH_MSG_INFO ("finished processing successfully");

  return EXIT_SUCCESS;
}
