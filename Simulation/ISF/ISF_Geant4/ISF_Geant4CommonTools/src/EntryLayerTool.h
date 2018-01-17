/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryLayerTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H
#define ISF_GEANT4COMMONTOOLS_ENTRYLAYERTOOL_H 1

// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
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

  class EntryLayerTool : public extends<AthAlgTool, ISF::IEntryLayerTool, IIncidentListener> {

  public:
    /** Constructor with parameters */
    EntryLayerTool( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~EntryLayerTool();

    /** Athena algtool's Hooks */
    StatusCode  initialize();
    StatusCode  finalize();

    /** handle for incident service */
    void handle(const Incident& inc);

    /** Check if given particle passes the EntryLayer filters */
    virtual bool passesFilters( const ISFParticle& particle);

    /** Identify the corresponding entry layer for the given particle (may return 
        ISF::fUnsetEntryLayere if particle is not on an entry layer surface) */
    virtual ISF::EntryLayer identifyEntryLayer( const ISFParticle& particle);

    /** Add the given particle to the corresponding Entry/Exit layer if applicable */
    virtual ISF::EntryLayer registerParticle( const ISF::ISFParticle &particle,
                                              ISF::EntryLayer entryLayer);
 

  private:
    /** used to setup a TrackRecordCollection on storegate */
    TrackRecordCollection *setupSGCollection(const std::string &name);

    /*  Incident Service */
    ServiceHandle<IIncidentSvc>               m_incidentSvc;

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
