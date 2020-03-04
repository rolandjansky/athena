#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODJet/JetContainer.h"

#include "TFile.h"
#include "TTree.h"

int main (int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << ": <DAOD> <jet collection>"
              << std::endl;
    return 1;
  }
  std::string file = argv[1];
  std::string jets_name = argv[2];

  // The name of the application:
  const char *const APP_NAME = "BTagTestDumper";

  // Set up the environment:
  RETURN_CHECK( APP_NAME, xAOD::Init() );

  // Set up the event object:
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // Open the file:
  std::unique_ptr<TFile> ifile(TFile::Open(file.c_str(), "READ"));
  if ( ! ifile.get() || ifile->IsZombie()) {
    Error( APP_NAME, "Couldn't open file: %s", file.c_str() );
    return 1;
  }
  Info( APP_NAME, "Opened file: %s", file.c_str() );

  // Connect the event object to it:
  RETURN_CHECK( APP_NAME, event.readFrom(ifile.get()) );

  unsigned long long nbad = 0;
  unsigned long long ngood = 0;
  unsigned long long entries = event.getEntries();
  for (unsigned long long entry = 0; entry < entries; ++entry) {
    // Load the event:
    if (event.getEntry(entry) < 0) {
      Error( APP_NAME, "Couldn't load entry %lld from file: %s",
             entry, file.c_str() );
      return 1;
    }
    const xAOD::JetContainer *jets = nullptr;
    RETURN_CHECK( APP_NAME, event.retrieve(jets, jets_name) );
    for (const xAOD::Jet *const jet : *jets) {
      const xAOD::BTagging* btag = jet->btagging();
      if (!btag) {
        nbad++;
      } else {
        ngood++;
      }
    }
  }
  std::cout << "found " << nbad << " jets with no valid btagging link "
            << "in " << nbad + ngood << " jets" << std::endl;
  return nbad > 0;
}
