/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackParticleCreatorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECREATORTOOL_H
#define ITRKTRACKPARTICLECREATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TrkParticleBase/TrackParticleBase.h" // to know TrackParticleOrigin enum
#include "TrkTrack/TrackCollection.h"
#include "AthLinks/ElementLink.h"

#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
namespace Rec
{
  class TrackParticle;
}

namespace InDet{
  class BeamSpotData;
}

namespace Trk 
{
  class Track;
  class VxCandidate;
  class PRDtoTrackMap;

  /** @brief Interface for constructing TrackParticles from complete tracks.

      @author Edward Moyse, Andreas Wildauer <http://consult.cern.ch/xwho>
  */
  class ITrackParticleCreatorTool : virtual public IAlgTool {

  public:
    /** InterfaceID
     */
    DeclareInterfaceID(ITrackParticleCreatorTool, 1, 0);

    /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee 
        i.e. the TrackParticle will not be complete 
        @param track Pointer to a valid track (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted) 
        @param vxCandidate Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted) 
        @param prtOrigin  
    */
    virtual Rec::TrackParticle* createParticle(
      const EventContext& ctx,
      const Trk::Track* track,
      const Trk::VxCandidate* vxCandidate = nullptr,
      Trk::TrackParticleOrigin prtOrigin = Trk::NoVtx) const = 0;

    Rec::TrackParticle* createParticle(
      const Trk::Track* track,
      const Trk::VxCandidate* vxCandidate = nullptr,
      Trk::TrackParticleOrigin prtOrigin = Trk::NoVtx) const
    {
      return createParticle(
        Gaudi::Hive::currentContext(), track, vxCandidate, prtOrigin);
    }

    /** Method to construct a xAOD::TrackParticle from a Rec::TrackParticle.
        @param track particle
        @param TrackParticleContainer needed to have an AuxStore, if provided
       particle will be added to store which takes ownership
    */
    virtual xAOD::TrackParticle* createParticle(
      const EventContext& ctx,
      const Rec::TrackParticle& trackParticle,
      xAOD::TrackParticleContainer* container = nullptr) const = 0;

    xAOD::TrackParticle* createParticle(
      const Rec::TrackParticle& trackParticle,
      xAOD::TrackParticleContainer* container = nullptr) const
    {
      return createParticle(
        Gaudi::Hive::currentContext(), trackParticle, container);
    }

    /** Method to construct a TrackParticle from a passed Track.
        @param track element link to the track is not set, use the method with the element link if you want the link as well
        @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
        @param xAOD::Vertex Pointer to a  vxCandidate . Ownership is not taken 
        @param prtOrigin
        @param prd_to_track_map an optional PRD-to-track map to compute shared hits.
    */
    virtual xAOD::TrackParticle* createParticle(
      const EventContext& ctx,
      const Trk::Track& track,
      xAOD::TrackParticleContainer* container = nullptr,
      const xAOD::Vertex* vxCandidate = nullptr,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      const Trk::PRDtoTrackMap* prd_to_track_map = nullptr) const = 0;

    xAOD::TrackParticle* createParticle(
      const Trk::Track& track,
      xAOD::TrackParticleContainer* container = nullptr,
      const xAOD::Vertex* vxCandidate = nullptr,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      const Trk::PRDtoTrackMap* prd_to_track_map = nullptr) const
    {
      return createParticle(Gaudi::Hive::currentContext(),
                            track,
                            container,
                            vxCandidate,
                            prtOrigin,
                            prd_to_track_map);
    }

    /** Method to construct a TrackParticle from a passed Track. 
        @param track element link to a valid track (i.e. do not pass a zero!).
        @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
        @param xAOD::Vertex Pointer to a  vxCandidate.
        @param prtOrigin
        @param prd_to_track_map an optional PRD-to-track map to compute shared hits.
    */
    virtual xAOD::TrackParticle* createParticle(
      const EventContext& ctx,
      const ElementLink<TrackCollection>& trackLink,
      xAOD::TrackParticleContainer* container = nullptr,
      const xAOD::Vertex* vxCandidate = nullptr,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      const Trk::PRDtoTrackMap* prd_to_track_map = nullptr) const = 0;

    xAOD::TrackParticle* createParticle(
      const ElementLink<TrackCollection>& trackLink,
      xAOD::TrackParticleContainer* container = nullptr,
      const xAOD::Vertex* vxCandidate = nullptr,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      const Trk::PRDtoTrackMap* prd_to_track_map = nullptr) const
    {
      return createParticle(Gaudi::Hive::currentContext(),
                            trackLink,
                            container,
                            vxCandidate,
                            prtOrigin,
                            prd_to_track_map);
    }

    /** create a xAOD::TrackParticle out of constituents (please don't use this
     * - it will eventually be removed) */
    virtual xAOD::TrackParticle* createParticle(
      const EventContext& ctx,
      const Perigee* perigee,
      const FitQuality* fq,
      const TrackInfo* trackInfo,
      const TrackSummary* summary,
      const std::vector<const Trk::TrackParameters*>& parameters,
      const std::vector<xAOD::ParameterPosition>& positions,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      xAOD::TrackParticleContainer* container = nullptr) const = 0;

    xAOD::TrackParticle* createParticle(
      const Perigee* perigee,
      const FitQuality* fq,
      const TrackInfo* trackInfo,
      const TrackSummary* summary,
      const std::vector<const Trk::TrackParameters*>& parameters,
      const std::vector<xAOD::ParameterPosition>& positions,
      xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
      xAOD::TrackParticleContainer* container = nullptr) const
    {
      return createParticle(Gaudi::Hive::currentContext(),
                            perigee,
                            fq,
                            trackInfo,
                            summary,
                            parameters,
                            positions,
                            prtOrigin,
                            container);
    }

    /** Convenience method to retrieve Beamspot Data object */
    virtual const InDet::BeamSpotData* CacheBeamSpotData(
      const EventContext& ctx) const = 0;
  };

} // end of namespace

#endif 
