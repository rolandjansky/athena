#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingUtilities.h"
#include "AsgMessaging/MessageCheck.h"

#include "TFile.h"
#include "TTree.h"
#include "TError.h"

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]) {

  ANA_CHECK_SET_TYPE (int);
  using namespace asg::msgUserCode;
  gErrorIgnoreLevel = kError;

  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << ": <DAOD> <jet collection>"
              << std::endl;
    return 1;
  }
  std::string file = argv[1];
  std::string jets_name = argv[2];

  // The name of the application:
  const std::string APP_NAME = "BTagTestDumper";

  // Set up the environment:
  ANA_CHECK( xAOD::Init() );

  // Set up the event object:
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // Open the file:
  std::unique_ptr<TFile> ifile(TFile::Open(file.c_str(), "READ"));
  if ( ! ifile.get() || ifile->IsZombie()) {
    std::cerr << "Couldn't open file: " << file << std::endl;
    return 1;
  }

  // Connect the event object to it:
  ANA_CHECK( event.readFrom(ifile.get()) );

  unsigned long long nbad = 0;
  unsigned long long ngood = 0;
  unsigned long long entries = event.getEntries();
  for (unsigned long long entry = 0; entry < entries; ++entry) {
    // Load the event:
    if (event.getEntry(entry) < 0) {
      std::cerr << "Couldn't load entry " << entry << " from file"
                << file << std::endl;
      return 1;
    }
    const xAOD::JetContainer *jets = nullptr;
    ANA_CHECK( event.retrieve(jets, jets_name) );
    for (const xAOD::Jet *const jet : *jets) {
      const xAOD::BTagging *btag = xAOD::BTaggingUtilities::getBTagging( *jet );
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
