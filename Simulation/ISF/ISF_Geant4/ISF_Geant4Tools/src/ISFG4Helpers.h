/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_ISFG4HELPERS_H
#define ISF_GEANT4TOOLS_ISFG4HELPERS_H

// Barcode includes
#include "BarcodeInterfaces/Barcode.h"

// ISF Includes
#include "ISF_Event/ISFParticle.h"

// forward declarations
namespace HepMC {
  class GenParticle;
}
namespace ISF {
  class ITruthBinding;
}
class VTrackInformation;
class G4Track;

namespace iGeant4 {

  class ISFG4Helpers {

  public:
    /** dummy constructor */
    ISFG4Helpers();
    /** dummy destructor */
    ~ISFG4Helpers();

    /** convert the given G4Track into an ISFParticle */
    static ISF::ISFParticle* convertG4TrackToISFParticle(const G4Track& aTrack,
                                                         const ISF::ISFParticle& parent,
                                                         ISF::ITruthBinding* truth = nullptr);

    /** convert the given G4Track into an ISFParticle */
    static ISF::ISFParticle* convertG4TrackToISFParticle(const G4Track& aTrack,
                                                         const ISF::DetRegionSvcIDPair& regionSimSvcPair,
                                                         const HepMC::GenParticle* primaryTruthParticle,
                                                         ISF::ITruthBinding* truth = nullptr);

    /** get the ParticleBarcode corresponding to the given G4Track */
    static Barcode::ParticleBarcode getParticleBarcode(const G4Track& aTrack);

    /** return a valid UserInformation object of the G4Track for use within the ISF */
    static VTrackInformation* getISFTrackInfo(const G4Track& aTrack);

    /** link the given G4Track to the given ISFParticle */
    static void setG4TrackInfoFromBaseISFParticle( G4Track& aTrack,
                                                   const ISF::ISFParticle& baseIsp,
                                                   bool setReturnToISF=false );

  private:

  };
}

#endif // ISF_GEANT4TOOLS_ISFG4HELPERS_H
