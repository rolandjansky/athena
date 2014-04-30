/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackParticleCreatorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECREATORTOOL_H
#define ITRKTRACKPARTICLECREATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
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

namespace Trk 
{
    class Track;
    class VxCandidate;

    static const InterfaceID IID_ITrackParticleCreatorTool("Trk::ITrackParticleCreatorTool", 1, 0);

    /** @brief Interface for constructing TrackParticles from complete tracks.

    @author Edward Moyse, Andreas Wildauer <http://consult.cern.ch/xwho>
    */
    class ITrackParticleCreatorTool : virtual public IAlgTool {

      public:
        static const InterfaceID& interfaceID( ) ;


      /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee 
          i.e. the TrackParticle will not be complete 
          @param track Pointer to a valid track (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted) 
          @param vxCandidate Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted) 
          @param prtOrigin  
          @warning In my opinion, the interface is not optimal - we're not taking ownership of the Trk::Track or Vx::Candidate,  
          so they should be passed by reference. 
      */ 
      virtual Rec::TrackParticle* createParticle( const Trk::Track* track, 
                                                  const Trk::VxCandidate* vxCandidate = 0, 
                                                  Trk::TrackParticleOrigin prtOrigin = Trk::NoVtx)=0; 

        /** Method to construct a xAOD::TrackParticle from a Rec::TrackParticle.
	    @param track particle 
	    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
        */
        virtual xAOD::TrackParticle* createParticle( const Rec::TrackParticle& trackParticle,  xAOD::TrackParticleContainer* container=0 ) const = 0;

        /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee
	    i.e. the TrackParticle will not be complete
	    @param track element link to the track is not set, use the method with the element link if you want the link as well
	    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
	    @param xAOD::Vertex Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
	    @param prtOrigin 
        */
        virtual xAOD::TrackParticle* createParticle( const Trk::Track& track,
						     xAOD::TrackParticleContainer* container=0,
						     const xAOD::Vertex* vxCandidate = 0,
						     xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis) const =0;
      
        /** Method to construct a TrackParticle from a passed Track. Currently, it will ONLY fill the MeasuredPerigee
	    i.e. the TrackParticle will not be complete
	    @param track element link to a valid track (i.e. do not pass a zero!). 
	    @param TrackParticleContainer needed to have an AuxStore, if provided particle will be added to store which takes ownership
	    @param xAOD::Vertex Pointer to a valid vxCandidate (i.e. do not pass a zero!). Ownership is not taken (i.e. it will not be deleted)
	    @param prtOrigin 
        */
        virtual xAOD::TrackParticle* createParticle( const ElementLink<TrackCollection>& trackLink,
						     xAOD::TrackParticleContainer* container=0,
						     const xAOD::Vertex* vxCandidate = 0,
						     xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis) const =0;

      /** create a xAOD::TrackParticle out of constituents (please don't use this - it will eventually be removed) */
      virtual xAOD::TrackParticle* createParticle( const Perigee* perigee, const FitQuality* fq, 
						   const TrackInfo* trackInfo, const TrackSummary* summary,
						   const std::vector<const Trk::TrackParameters*>& parameters,
                                                   const std::vector< xAOD::ParameterPosition>& positions,  
						   xAOD::ParticleHypothesis prtOrigin = xAOD::noHypothesis,
						   xAOD::TrackParticleContainer* container = 0 ) const = 0;
      

    };

    inline const InterfaceID& Trk::ITrackParticleCreatorTool::interfaceID()
    {
      return IID_ITrackParticleCreatorTool; 
    }

} // end of namespace

#endif 
