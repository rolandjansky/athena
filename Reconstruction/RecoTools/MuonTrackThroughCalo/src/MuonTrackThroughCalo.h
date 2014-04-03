/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackThroughCalo.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef REC_MUONTRACKTHROUGHCALO_H
#define REC_MUONTRACKTHROUGHCALO_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "RecoToolInterfaces/IMuonTrackThroughCalo.h"
#include "GaudiKernel/ToolHandle.h"
#include "Particle/TrackParticle.h"

namespace Trk {
  class IExtrapolator;
  class Surface;
}

namespace Rec {

  /** @class MuonTrackThroughCalo
    
  Implementation of the class MuonTrackThroughCalo. This class helps
  muon algorithms express the track parameters of the muon at different places
  in the ATLAS detector. It does so using the @c Extrapolator with specific,
  muon-specific configurations. Two extrapolator configurations are available
  allowing to use either a parameterization of the muon energy loss or a
  combination of the energy loss parameterization and the measurement in the
  calorimeter.

  @author David.Lopez@cern.ch
*/
  class MuonTrackThroughCalo : public AthAlgTool,
    virtual public IMuonTrackThroughCalo {
      
  public:
      
      /** AlgTool like constructor */
      MuonTrackThroughCalo(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~MuonTrackThroughCalo();
      
      /** AlgTool initialize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** <b> Transport track to the @c PerigeeSurface at (x,y) = (0,0)</b> 
	  
      The extrapolation is assumed to be opposite to the momentum direction 
      by default,
	  so results are only correct for particles coming from the interaction
	  point. If useMeasurement is true, the measurement in the Calorimeter
	  is used. For use with cosmics, the third parameter 
	  propagationDirection can be used.
      */
      const Trk::TrackParameters* extrapolateToPerigee (const Trk::TrackParameters& parms,
							bool useMeasurement,
							Trk::PropDirection propagationDirection = Trk::oppositeMomentum);

      /** <b> Transport track to the entrance of the Muon Spectrometer</b> 
	
      The extrapolation is assumed to be along the momentum direction,
	  so results are only correct for particles coming from the interaction
	  point. useMeasurement has the same meaning as in 
	  @c extrapolateToPerigee
      */
      const Trk::TrackParameters* extrapolateToMS (const Trk::TrackParameters& parms,
						   bool useMeasurement);


      /** <b> Transport track to the ID/Calorimeter interface </b> 
	
      The extrapolation is assumed to be along the momentum direction.
	  Measurement in the calorimeter is never used when calling this 
	  method.
      */
      const Trk::TrackParameters* forwardExtrapolateToID (const Trk::TrackParameters& parms);

      /** <b> Transport track to the ID/Calorimeter interface </b> 
	
      The extrapolation is assumed to be opposite to the momentum direction.
	  useMeasurement has the same meaning as in @c extrapolateToPerigee
      */
      const Trk::TrackParameters* backwardExtrapolateToID (const Trk::TrackParameters& parms,
							   bool useMeasurement);      
      
  private:      
      
      const Trk::TrackParameters* extrapolateToID (const Trk::TrackParameters& parms,
						   Trk::PropDirection prop,
						   bool useMeasurement = false);

      const Trk::TrackParameters* extrapolateToSurfaces (const Trk::TrackParameters& parms,
							 bool useMeasurement,
							 std::vector<Trk::Surface*> surfaces,
							 Trk::PropDirection prop,
							 bool id = false,
							 bool checkHemisphereChange = false);


      ToolHandle< Trk::IExtrapolator >  m_staticExtrapolator; //!< Static extrapolator: most stable against navigation breaks
      ToolHandle< Trk::IExtrapolator >  m_dynamicParamExtrapolator; //!< Dynamic parameterized extrapolator: 
      ToolHandle< Trk::IExtrapolator >  m_dynamicCombiExtrapolator; //!< Dynamic combined extrapolator: 

    };
  
} // end of namespace

#endif //REC_MUONTRACKTHROUGHCALO_H


