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

    AtlasDetDescr::AtlasRegion nextGeoId(const G4Step* aStep);

    bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 ) const;

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

    bool m_geant4OnlyMode;

    int m_truthVolLevel; //!< Which level in the hierarchy are our truth volumes?

    std::map<std::string, int, std::less<std::string> > m_entryLayerMap;

    std::map<int, ISF::ISFParticle*> m_parentISPmap;
  };
   
}

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTION_H
