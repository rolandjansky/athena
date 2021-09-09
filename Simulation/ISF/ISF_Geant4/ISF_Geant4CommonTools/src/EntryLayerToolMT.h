/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOLMT_H
#define ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOLMT_H 1

// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Athena
#include "AthenaBaseComps/AthAlgTool.h"

// ISF interfaces
#include "ISF_Interfaces/IParticleFilter.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Interfaces/IEntryLayerTool.h"
#include "G4AtlasTools/ThreadLocalHolder.h"

// TrackRecordCollection (and TrackRecord)
#include "TrackRecord/TrackRecordCollection.h"

// stl includes
#include <string>

namespace ISF {
  class ISFParticle;
}

namespace ISF {

  /** use this typedef to make the code structure more clean*/
  typedef ToolHandleArray<ISF::IParticleFilter>    ParticleFilterArray;

  /** @class EntryLayerTool


      @author Elmar.Ritsch -at- cern.ch
  */

  class EntryLayerToolMT : public extends<AthAlgTool, ISF::IEntryLayerTool> {

  public:
    /** Constructor with parameters */
    EntryLayerToolMT( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    virtual ~EntryLayerToolMT() = default;

    /** Athena algtool's Hooks */
    virtual StatusCode  initialize() override final;

    // /** handle for incident service */
    // void handle(const Incident& inc);

    /** Check if given particle passes the EntryLayer filters */
    virtual bool passesFilters( const ISFParticle& particle) override final;

    /** Identify the corresponding entry layer for the given particle (may return
        ISF::fUnsetEntryLayere if particle is not on an entry layer surface) */
    virtual ISF::EntryLayer identifyEntryLayer( const ISFParticle& particle) override final;

    /** Add the given particle to the corresponding Entry/Exit layer if applicable */
    virtual ISF::EntryLayer registerParticle( const ISF::ISFParticle &particle,
                                              ISF::EntryLayer entryLayer) override final;

    /** Register the TrackRecordCollection pointer for a layer */
    virtual StatusCode registerTrackRecordCollection(TrackRecordCollection* collection, EntryLayer layer) override final;

    virtual void setupEvent() { return; }

  private:
    /** GeoIDSvc will be used to determine the entry layer surface, the particle is on */
    ServiceHandle<ISF::IGeoIDSvc>             m_geoIDSvc{this, "GeoIDSvc", "GeoIDSvc", "AthenaService used to indentify sub-detector by (x,y,z) coordintes."};
    ISF::IGeoIDSvc                           *m_geoIDSvcQuick{};

    /** Array of filters to decide whether a particle is added to the Entry/Exit layer */
    ParticleFilterArray                       m_particleFilterHandle{this, "ParticleFilters", {}, "ISF Particle filters, defining whether a particle will be stored or not."};
    ISF::IParticleFilter                    **m_particleFilter{};
    size_t                                    m_numParticleFilters{0};

    /** The entry layer collections */
    thread_utils::ThreadLocalOwner< std::array<TrackRecordCollection*, ISF::fNumAtlasEntryLayers> > m_collectionHolder;
    std::string                               m_volumeName[ISF::fNumAtlasEntryLayers];
  };

}

#endif //> !ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H
