/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTION_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTION_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"

#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"

#include "ISF_Event/EntryLayer.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

namespace ISF {
  class ISFParticle;
  class IParticleBroker;
}
class G4LogicalVolume;

namespace HepMC {
  class GenParticle;
}

namespace iGeant4 {

  class TrackProcessorUserAction: virtual public ITrackProcessorUserAction, public AthAlgTool {

  public:
    TrackProcessorUserAction(const std::string& type,
                             const std::string& name,
                             const IInterface* parent);
    virtual ~TrackProcessorUserAction() {}

    StatusCode initialize();
    StatusCode finalize();

    void beginOfAtlasEvent();

    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);

    void SteppingAction(const G4Step*);

    void PreUserTrackingAction(const G4Track* aTrack);
    void PostUserTrackingAction(const G4Track* aTrack);

  private:

    ISF::EntryLayer entryLayer(const G4Step* aStep);

    ISF::ISFParticle* newISFParticle(G4Track* aTrack, const ISF::ISFParticle* parent, AtlasDetDescr::AtlasRegion  nextGeoID);

    /** kills the given G4Track, converts it into an ISFParticle and returns it to the ISF particle broker */
    void returnParticleToISF( G4Track *aTrack, ISF::ISFParticle *parentISP, AtlasDetDescr::AtlasRegion nextGeoID );

    HepMC::GenParticle* findMatchingDaughter(HepMC::GenParticle* parent, bool verbose) const;

    AtlasDetDescr::AtlasRegion nextGeoId(const G4Step* aStep);

    bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 );

    ServiceHandle<ISF::IParticleBroker>  m_particleBroker;

    /** access to the central ISF GeoID serice*/
    ServiceHandle<ISF::IGeoIDSvc>        m_geoIDSvc;      //!< athena service handle
    ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead

    //std::map<G4Int,ISF::ISFParticle*> m_idToStackParticleMap;

    double m_idR, m_idZ;
    double m_caloRmean,  m_caloZmean;
    double m_muonRmean, m_muonZmean;
    double m_cavernRmean, m_cavernZmean;
    int m_volumeOffset;
    int m_minHistoryDepth;
    bool m_hasCavern;

    bool m_geant4OnlyMode;

    int m_truthVolLevel; //!< Which level in the hierarchy are our truth volumes?

    std::map<std::string, int, std::less<std::string> > m_entryLayerMap;

    std::map<int, ISF::ISFParticle*> m_parentISPmap;

    /** for keeping track of the currently processed G4Track and its corresponding ISFParticle */
    int                                 m_curTrackID;  //!< the TrackID of the currently processed G4Track
    ISF::ISFParticle                   *m_curISP;      //!< the corresponding ISFParticle to this G4Track

    /** properties to define which particles are returned to ISF */
    double                              m_passBackEkinThreshold;
    bool                                m_killBoundaryParticlesBelowThreshold;

  };

}

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTION_H
