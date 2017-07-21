/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONPASSBACK_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONPASSBACK_H

#include "TrackProcessorUserActionBase.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ISF_Interfaces/IGeoIDSvc.h"

#include <string>

// forward declarations

// Geant4
class G4LogicalVolume;

namespace ISF {
  class ISFParticle;
  class IParticleBroker;
  class ITruthBinding;
}

namespace HepMC {
  class GenParticle;
}

namespace iGeant4 {

  class TrackProcessorUserActionPassBack final: public TrackProcessorUserActionBase {

  public:
    TrackProcessorUserActionPassBack(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);
    virtual ~TrackProcessorUserActionPassBack() {}

    StatusCode initialize();
    StatusCode finalize();

  private:
    /** Called by the base class after the G4Track->ISFParticle association
     *  has been established */
    void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override final;

    /** create a new TruthBinding object for the given G4Track (may return 0 if unable) */
    ISF::TruthBinding* newTruthBinding(const G4Track* aTrack, HepMC::GenParticle* truthParticle) const;

    ISF::ISFParticle* newISFParticle(G4Track* aTrack,
                                     const ISF::ISFParticle* parent,
                                     HepMC::GenParticle* truthParticle,
                                     AtlasDetDescr::AtlasRegion nextGeoID);

    /** kills the given G4Track, converts it into an ISFParticle and returns it to the ISF particle broker */
    void returnParticleToISF( G4Track* aTrack,
                              const ISF::ISFParticle* parentISP,
                              HepMC::GenParticle* truthParticle,
                              AtlasDetDescr::AtlasRegion nextGeoID );

    /** handle for the ISF Particle Broker to allow us to push back particles to the ISF */
    ServiceHandle<ISF::IParticleBroker>  m_particleBroker;      //!< athena tool handle
    ISF::IParticleBroker                *m_particleBrokerQuick; //!< quickaccess avoiding gaudi ovehead

    /** access to the central ISF GeoID serice*/
    ServiceHandle<ISF::IGeoIDSvc>        m_geoIDSvc;      //!< athena service handle
    ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead

    /** properties to define which particles are returned to ISF */
    double                              m_passBackEkinThreshold;
    bool                                m_killBoundaryParticlesBelowThreshold;

  };

}

namespace G4UA{

  namespace iGeant4{

    class TrackProcessorUserActionPassBack: public G4UA::iGeant4::TrackProcessorUserActionBase
    {

    public:

      struct Config
      {
        ServiceHandle<ISF::IParticleBroker>  particleBroker=ServiceHandle<ISF::IParticleBroker>("","TrackProcessorUserActionPassBack");
        ServiceHandle<ISF::IGeoIDSvc>        geoIDSvc=ServiceHandle<ISF::IGeoIDSvc>("","TrackProcessorUserActionPassBack");
        /** properties to define which particles are returned to ISF */
        double                              passBackEkinThreshold=0.05;
        bool                                killBoundaryParticlesBelowThreshold=false;
        unsigned int                        verboseLevel=0;
      };

      TrackProcessorUserActionPassBack(const Config& config);
    private:
      /** Called by the base class after the G4Track->ISFParticle association
       *  has been established */
      void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override final;

      Config m_config;

      /** create a new TruthBinding object for the given G4Track (may return 0 if unable) */
      ISF::TruthBinding* newTruthBinding(const G4Track* aTrack, HepMC::GenParticle* truthParticle) const;

      ISF::ISFParticle* newISFParticle(G4Track* aTrack,
                                       const ISF::ISFParticle* parent,
                                       HepMC::GenParticle* truthParticle,
                                       AtlasDetDescr::AtlasRegion nextGeoID);

      /** kills the given G4Track, converts it into an ISFParticle and returns it to the ISF particle broker */
      void returnParticleToISF( G4Track *aTrack,
                                const ISF::ISFParticle *parentISP,
                                HepMC::GenParticle* truthParticle,
                                AtlasDetDescr::AtlasRegion nextGeoID );

      ISF::IParticleBroker                *m_particleBrokerQuick; //!< quickaccess avoiding gaudi ovehead
      ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead

    }; // class TrackProcessorUserActionPassBack

  }// iGeant4

}// G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONPASSBACK_H
