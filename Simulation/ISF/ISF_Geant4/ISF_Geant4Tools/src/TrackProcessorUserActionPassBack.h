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

    /** Called by the base class after the G4Track->ISFParticle association
     *  has been established */
    void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override;

  private:
    /** create a new TruthBinding object for the given G4Track (may return 0 if unable) */
    ISF::ITruthBinding* newTruthBinding(const G4Track* aTrack) const;

    ISF::ISFParticle* attachNewISFParticle(G4Track* aTrack,
                                           const ISF::ISFParticle* parent,
                                           AtlasDetDescr::AtlasRegion nextGeoID);

    /** kills the given G4Track, converts it into an ISFParticle and returns it to the ISF particle broker */
    void returnParticleToISF( G4Track *aTrack, ISF::ISFParticle *parentISP, AtlasDetDescr::AtlasRegion nextGeoID );

    HepMC::GenParticle* findMatchingDaughter(HepMC::GenParticle* parent, bool verbose) const;

    bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 );

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

#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{

  namespace iGeant4{

    class TrackProcessorUserActionPassBack:
    public AthMessaging, public G4UA::iGeant4::TrackProcessorUserActionBase, public IBeginRunAction
    {
      
    public:
      
      struct Config
      {
	ServiceHandle<ISF::IParticleBroker>  particleBroker=ServiceHandle<ISF::IParticleBroker>("","TrackProcessorUserActionPassBack");
	ServiceHandle<ISF::IGeoIDSvc>        geoIDSvc=ServiceHandle<ISF::IGeoIDSvc>("","TrackProcessorUserActionPassBack");
	/** properties to define which particles are returned to ISF */
	double                              passBackEkinThreshold=0.05; 
	bool                                killBoundaryParticlesBelowThreshold=false;
      };
      
      TrackProcessorUserActionPassBack(const Config& config);
      virtual void beginOfRun(const G4Run*) override;

      /** Called by the base class after the G4Track->ISFParticle association
       *  has been established */
      void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override;
      
      
      
    private:
      Config m_config;
      
      /** create a new TruthBinding object for the given G4Track (may return 0 if unable) */
      ISF::ITruthBinding* newTruthBinding(const G4Track* aTrack) const;
      
      ISF::ISFParticle* attachNewISFParticle(G4Track* aTrack,
					     const ISF::ISFParticle* parent,
					     AtlasDetDescr::AtlasRegion nextGeoID);
      
      /** kills the given G4Track, converts it into an ISFParticle and returns it to the ISF particle broker */
      void returnParticleToISF( G4Track *aTrack, ISF::ISFParticle *parentISP, AtlasDetDescr::AtlasRegion nextGeoID );
      
      HepMC::GenParticle* findMatchingDaughter(HepMC::GenParticle* parent, bool verbose) const;
      
      bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 );
      ISF::IParticleBroker                *m_particleBrokerQuick; //!< quickaccess avoiding gaudi ovehead
      ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead
      
    }; // class TrackProcessorUserActionPassBack
    
  }// iGeant4
  
}// G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONPASSBACK_H
