/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_PARTICLEEXTRAPOLATIONMAP_H
#define REC_PARTICLEEXTRAPOLATIONMAP_H

#include <vector>
#include <map>
#include "GaudiKernel/GaudiException.h"
#include "Particle/TrackParticle.h"
#include "TrkParameters/TrackParameters.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Trk { class Track; }

namespace RecIntersect {

    /** enum identifying at which detector boundary parameters are expressed */
    enum LayerType { PreSampler = 0,
		      Strips,
		      Middle,
		      Back,
		      Tile,
		      InDetFirstHit,
		      InDetLastHit,
		      MSEntrance,
		      MSFirstHit,
		      NLayerTypes};

}

namespace Rec {

  typedef std::pair<const Trk::TrackParameters*, RecIntersect::LayerType>  IdentifiedTrackParameters;
  typedef std::vector<IdentifiedTrackParameters>      ParticleExtrapolationVector;
  typedef std::map<const Rec::TrackParticle*, const ParticleExtrapolationVector*> ParticleMapType;
  typedef std::map<const Trk::Track*, const ParticleExtrapolationVector*> TrackMapType;

  /** @class ParticleExtrapolationMap
      @brief Map providing for each particle a pre-fabricated vector of parameters
             along an outside-extrapolated inner detector track.
             The main aim is to save CPU time. The map keys are particle and track,
             the latter filled from the particle's link to the track.
             For HLT uses, a version understanding only Trk::Track is supported.
             The class hides the implementation details (like the STL map) and the
             fact that the map has ownership on the vector of extrapolated parameters.

      @author Wolfgang Liebig */

  class ParticleExtrapolationMap {
  
  public:

    //! Default constructor
    ParticleExtrapolationMap() {}

    //! Custom destructor, handling the deletion of the parameters owned by the map.
    ~ParticleExtrapolationMap();

    //! method to fill up the map, HLT version: forced to be using tracks.
    void addExtrapolation(const Trk::Track& ,
                          const ParticleExtrapolationVector* ) throw (GaudiException);

    //! method to fill up the map: using particles
    void addExtrapolation(const Rec::TrackParticle&,
                          const ParticleExtrapolationVector* );

    //! method to test if the map can be used with Trk::Track as key
    bool haveTracks() const { return m_trackMap.size()>0; }

    //! method to test if the map can be used with Rec::Particle as key
    bool haveParticles() const { return m_particleMap.size()>0; }

    /* method to obtain at a certain detector layer/boundary the extrapolated parameters
       belonging to a Trk::Track */
    const Trk::TrackParameters* 
      getParameters(const Trk::Track&, RecIntersect::LayerType) const;

    /* method to obtain at a certain detector layer/boundary the extrapolated parameters
       belonging to a Rec::TrackParticle. */
    const Trk::TrackParameters* 
      getParameters(const Rec::TrackParticle&, RecIntersect::LayerType) const;

    
  private:
    //! the real internal STL map using particles as key
    ParticleMapType m_particleMap;
    //! the real internal STL map using tracks as key
    TrackMapType m_trackMap;

  };

}

// do 'clid <class name>' after an asetup and get the magic number
CLASS_DEF( Rec::ParticleExtrapolationMap , 71976586 , 1 )

#endif
