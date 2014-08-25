/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Interface for TrackThroughDetectorBoundaries
// ***************************************************************************

#ifndef REC_ITRACKTHROUGHDETECTORBOUNDARIES_H
#define REC_ITRACKTHROUGHDETECTORBOUNDARIES_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "Particle/ParticleExtrapolationMap.h"

static const InterfaceID IID_ITrackThroughDetectorBoundaries("ITrackThroughDetectorBoundaries", 1, 0);

namespace Rec {
  /** @class ITrackThroughDetectorBoundaries
      @brief Interface for producing a vector of parameters, each at a designated 
      detector boundary or layer surface, corresponding to the outwards movement
      of a particle.
  
      @author Wolfgang Liebig

  */

  class ITrackThroughDetectorBoundaries : virtual public IAlgTool {
  public:

    /** Virtual destructor */
    virtual ~ITrackThroughDetectorBoundaries(){}

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID(){ return IID_ITrackThroughDetectorBoundaries; };

    /** Extrapolation stepping through the boundaries between detector systems and the main Calo layers. */
    virtual const Rec::ParticleExtrapolationVector* getParametersAcrossAtlas
      (const Trk::Track& trk,
       const Trk::ParticleHypothesis particle=Trk::muon,
       const Trk::PropDirection dir=Trk::alongMomentum) const = 0;

    /** Helper method to obtain from a track the two parameters corresponding to the
        first and last measurements in the ID. */
    virtual std::pair<const Trk::TrackParameters*, const Trk::TrackParameters*>
      getIndetEntryAndExitParameters(const Trk::Track&) const = 0;

  };


}

#endif // REC_ITRACKTHROUGHDETECTORBOUNDARIES_H
