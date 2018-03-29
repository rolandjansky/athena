/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONFULLG4_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONFULLG4_H

#include "TrackProcessorUserActionBase.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "AtlasDetDescr/AtlasRegion.h"

#include "ISF_Interfaces/IEntryLayerTool.h"
#include "ISF_Interfaces/IGeoIDSvc.h"

#include "ISF_Event/EntryLayer.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

// forward declarations

// Geant4
class G4LogicalVolume;

namespace G4UA{

  namespace iGeant4{

    class TrackProcessorUserActionFullG4 final: public ::G4UA::iGeant4::TrackProcessorUserActionBase {
    public:
      struct Config
      {
        Config() = delete;
        Config(const std::string& name):
          entryLayerTool(""),
          geoIDSvc("",name),
          truthVolLevel(1)
        {}
        /** access to the ISF Entry Layer tool which is used to record entry-layer collections */
        ToolHandle<ISF::IEntryLayerTool> entryLayerTool;
        /** access to the central ISF GeoID service*/
        ServiceHandle<ISF::IGeoIDSvc>    geoIDSvc;
        int truthVolLevel;
        unsigned int                     verboseLevel=0;
      };

      TrackProcessorUserActionFullG4(const Config& config);
      virtual ~TrackProcessorUserActionFullG4() {}

      virtual void PreUserTrackingAction(const G4Track*) override final;
    private:
      /** Called by the base class after the G4Track->ISFParticle association
       *  has been established */
      void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override final;

      Config m_config;

      ISF::EntryLayer entryLayer(const G4Step* aStep);

      /** access to the ISF Entry Layer tool which is used to record entry-layer collections */
      ISF::IEntryLayerTool                *m_entryLayerToolQuick; //!< quickaccess avoiding gaudi ovehead

      /** access to the central ISF GeoID serice*/
      ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead


      std::map<std::string, int, std::less<std::string> > m_entryLayerMap;

      AtlasDetDescr::AtlasRegion m_nextGeoID;
      const G4Track* m_currentTrack;

    }; // class TrackProcessorUserActionFullG4

  }// iGeant4

} // G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONFULLG4_H
