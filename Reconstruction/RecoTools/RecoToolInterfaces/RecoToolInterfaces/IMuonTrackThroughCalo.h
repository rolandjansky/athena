/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Interface for MuonTrackThroughCalo
// ***************************************************************************

#ifndef IMUONTRACKTHROUGHCALO_H
#define IMUONTRACKTHROUGHCALO_H

//<<<<<< INCLUDES >>>>>>
#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PropDirection.h"

namespace Rec {

  static const InterfaceID IID_IMuonTrackThroughCalo("IMuonTrackThroughCalo", 1, 0);

  /** @class IMuonTrackThroughCalo
      @brief Interface for extrapolating muons inwards and outwards while controlling the
             way how the energy loss in the calorimeter is taken into account.
             The implementation is in Reconstruction/RecoTools/MuonTrackThroughCalo
        
      @author David.Lopez@cern.ch
  */

  class IMuonTrackThroughCalo : virtual public IAlgTool {
  public:

    /** Virtual destructor */
    virtual ~IMuonTrackThroughCalo(){}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID(){ return IID_IMuonTrackThroughCalo; };

    virtual const Trk::TrackParameters* extrapolateToPerigee (const Trk::TrackParameters& parms,
							      bool useMeasurement,
							      Trk::PropDirection propagationDirection 
							      = Trk::oppositeMomentum) = 0;

    virtual const Trk::TrackParameters* extrapolateToMS (const Trk::TrackParameters& parms,
							 bool useMeasurement) = 0;

    virtual const Trk::TrackParameters* forwardExtrapolateToID (const Trk::TrackParameters& parms) = 0;

    virtual const Trk::TrackParameters* backwardExtrapolateToID (const Trk::TrackParameters& parms,
								 bool useMeasurement) = 0;

  };

}


#endif // IMUONTRACKTHROUGHCALO_H
