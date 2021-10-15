/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H
#define ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H 1

// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Athena
#include "AthenaBaseComps/AthAlgTool.h"

// ISF interface
#include "ISF_Interfaces/IEntryLayerTool.h"

// TrackRecordCollection (and TrackRecord)
#include "TrackRecord/TrackRecordCollection.h"

// stl includes
#include <string>

namespace ISF {
  class ISFParticle;
  class IParticleFilter;
  class IGeoIDSvc;
}

namespace ISF {

  /** use this typedef to make the code structure more clean*/
  typedef ToolHandleArray<ISF::IParticleFilter>    ParticleFilterArray;

  /** @class EntryLayerTool


      @author Elmar.Ritsch -at- cern.ch
  */

  class EntryLayerTool : public extends<AthAlgTool, ISF::IEntryLayerTool> {

  public:
    /** Constructor with parameters */
    EntryLayerTool( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~EntryLayerTool() = default;

    /** Athena algtool's Hooks */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  finalize() override final;

    /** Check if given particle passes the EntryLayer filters */
    virtual bool passesFilters( const ISFParticle& particle) override final;

    /** Identify the corresponding entry layer for the given particle (may return
        ISF::fUnsetEntryLayer if particle is not on an entry layer surface) */
    virtual ISF::EntryLayer identifyEntryLayer( const ISFParticle& particle) override final;

    /** Add the given particle to the corresponding Entry/Exit layer if applicable */
    virtual ISF::EntryLayer registerParticle( const ISF::ISFParticle &particle,
                                              ISF::EntryLayer entryLayer) override final;


    virtual void setupEvent() override final;

  private:
    /** used to setup a TrackRecordCollection on storegate */
    TrackRecordCollection *setupSGCollection(const std::string &name);

    /** GeoIDSvc will be used to determine the entry layer surface, the particle is on */
    ServiceHandle<ISF::IGeoIDSvc>             m_geoIDSvc;
    ISF::IGeoIDSvc                           *m_geoIDSvcQuick;

    /** Array of filters to decide whether a particle is added to the Entry/Exit layer */
    ParticleFilterArray                       m_particleFilterHandle;
    ISF::IParticleFilter                    **m_particleFilter;
    size_t                                    m_numParticleFilters;

    /** The entry layer collections */
    TrackRecordCollection                    *m_collection[ISF::fNumAtlasEntryLayers];
    std::string                               m_SGName[ISF::fNumAtlasEntryLayers];
    std::string                               m_volumeName[ISF::fNumAtlasEntryLayers];
  };

}

#endif //> !ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H
