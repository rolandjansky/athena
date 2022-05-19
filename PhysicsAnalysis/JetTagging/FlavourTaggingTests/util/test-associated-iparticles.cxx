// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "AsgMessaging/MessageCheck.h"

#include "TFile.h"
#include "TTree.h"
#include "TError.h"

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]) {

  ANA_CHECK_SET_TYPE (int);
  using namespace asg::msgUserCode;
  gErrorIgnoreLevel = kError;

  if (argc != 4) {
    std::cerr << "usage: " << argv[0] << ": <DAOD> <jet collection> <associated collection>"
              << "\n\n"
              << "Return codes:\n"
              << " -1: usage error\n"
              << "  1: broken association\n"
              << "  2: some associated objects are not tracks\n"
              << "  0: association works, is all tracks" << std::endl;
    return -1;
  }
  std::string file = argv[1];
  std::string jets_name = argv[2];
  std::string linked_particles_name = argv[3];

  // The name of the application:
  const std::string APP_NAME = "TestAssociatedIparticles";

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
  unsigned long long nIParticle = 0;
  unsigned long long nTrack = 0;
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
      std::vector<const xAOD::IParticle*> particles;
      if (!jet->getAssociatedObjects(linked_particles_name, particles)){
        nbad++;
      } else {
        ngood++;
        nIParticle += particles.size();
        for (const auto* part: particles) {
          if (dynamic_cast<const xAOD::TrackParticle*>(part)) {
            nTrack++;
          }
        }
      }
    }
  }
  std::cout << "Summary: found " << ngood << " out of " << nbad + ngood <<" jets with valid links to associated particles " << std::endl;
  std::cout << "Found " << nIParticle << " associated xAOD::IParticles, "
            << nTrack << " of which were tracks" << std::endl;
  if (nbad > 0) return 1;
  if (nIParticle > nTrack) return 2;
  return 0;
}
