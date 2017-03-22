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

namespace iGeant4 {

  class TrackProcessorUserActionFullG4 final: public TrackProcessorUserActionBase {

  public:
    TrackProcessorUserActionFullG4(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);
    virtual ~TrackProcessorUserActionFullG4() {}

    StatusCode initialize();
    StatusCode finalize();

  private:
    /** Called by the base class after the G4Track->ISFParticle association
     *  has been established */
    void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override final;

    ISF::EntryLayer entryLayer(const G4Step* aStep);

    AtlasDetDescr::AtlasRegion nextGeoId(const G4Step* aStep);

    bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 );

    /// get the next GeoID using only the geoIDSvc
    AtlasDetDescr::AtlasRegion getNextGeoIDFromSvc(const G4StepPoint& postStep) const;

    /** access to the ISF Entry Layer tool which is used to record entry-layer collections */
    ToolHandle<ISF::IEntryLayerTool>     m_entryLayerTool;      //!< athena tool handle
    ISF::IEntryLayerTool                *m_entryLayerToolQuick; //!< quickaccess avoiding gaudi ovehead

    /** access to the central ISF GeoID serice*/
    ServiceHandle<ISF::IGeoIDSvc>        m_geoIDSvc;      //!< athena service handle
    ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead

    bool m_hasCavern;

    int m_truthVolLevel; //!< Which level in the hierarchy are our truth volumes?

    std::map<std::string, int, std::less<std::string> > m_entryLayerMap;
  };

}

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

      virtual void preTracking(const G4Track*) override final;
    private:
      /** Called by the base class after the G4Track->ISFParticle association
       *  has been established */
      void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) override final;

      Config m_config;

      ISF::EntryLayer entryLayer(const G4Step* aStep);

      AtlasDetDescr::AtlasRegion nextGeoId(const G4Step* aStep);

      bool checkVolumeDepth( G4LogicalVolume * , int , int d=0 );

      /// get the next GeoID using only the geoIDSvc
      AtlasDetDescr::AtlasRegion getNextGeoIDFromSvc(const G4StepPoint& postStep) const;

      /** access to the ISF Entry Layer tool which is used to record entry-layer collections */
      ISF::IEntryLayerTool                *m_entryLayerToolQuick; //!< quickaccess avoiding gaudi ovehead

      /** access to the central ISF GeoID serice*/
      ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead

      bool m_hasCavern;

      std::map<std::string, int, std::less<std::string> > m_entryLayerMap;

      AtlasDetDescr::AtlasRegion m_nextGeoID;
      const G4Track* m_currentTrack;
      G4LogicalVolume * m_BPholder, * m_IDholder, * m_CALOholder , * m_MUholder, * m_TTRholder;
    }; // class TrackProcessorUserActionFullG4

  }// iGeant4

} // G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONFULLG4_H
