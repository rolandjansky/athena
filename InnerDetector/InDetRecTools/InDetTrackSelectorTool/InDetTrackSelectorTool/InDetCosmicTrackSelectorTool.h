/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrackSelectorTool_InDetCosmicTrackSelectorTool_H
#define InDetTrackSelectorTool_InDetCosmicTrackSelectorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

namespace Trk
{
  class ITrackSummaryTool;
  class Vertex;
  class TrackParticleBase;
  class Track;
}


namespace InDet
{

  class InDetCosmicTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool
  {

  public:

    StatusCode initialize();

    StatusCode finalize();

    InDetCosmicTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p);

    ~InDetCosmicTrackSelectorTool();

    bool decision(const Trk::Track& track,const Trk::Vertex* vertex) const;

    bool decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vertex) const;

    bool decision(const xAOD::TrackParticle&,const xAOD::Vertex*) const {
      ATH_MSG_WARNING("xAOD::TrackParticle selection not implemented yet");
      return false;
    }

  private:

    int getNSiHits(const Trk::Track* track, bool top) const;
    bool decision(const Trk::TrackParameters* track, const Trk::Vertex* vertex, const Trk::ParticleHypothesis) const;
    double m_maxZ0; //!< Maximum z0 of tracks
    double m_maxD0; //!< Maximum d0 of tracks
    double m_minPt; //!< Minimum pT of tracks
    int m_numberOfPixelHits; //!< Minimum number of Pixel hits
    int m_numberOfSCTHits; //!< Minimum number of SCT hits
    int m_numberOfTRTHits; //!< Minimum number of TRT hits
    int m_numberOfSiHits; //!< Minimum number of Silicon hits
    int m_numberOfSiHitsTop; //!< Minimum number of Silicon hits
    int m_numberOfSiHitsBottom; //!< Minimum number of Silicon hits

    ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool;
    bool m_trackSumToolAvailable;

    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    

  }; //end of class definitions
} //end of namespace definitions

#endif //TrkMultipleVertexSeedFinders_PVFindingTrackSelectoTool_H
