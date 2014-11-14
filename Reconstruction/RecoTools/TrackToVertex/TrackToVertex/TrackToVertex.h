/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToVertex.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////


#ifndef RECOTOOLS_TRACKTOVERTEX_H
#define RECOTOOLS_TRACKTOVERTEX_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// Trk
#include "ITrackToVertex/ITrackToVertex.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"

class IBeamCondSvc;
class IIncidentSvc;

namespace Rec {
  class TrackParticle;
}

namespace Trk {
  class Track;
  class IExtrapolator;
  class StraightLineSurface;
}
    
  /** @class TrackToVertex
    
     Standard Tool to extrapolate Track/TrackParticleBase to Vertex
     and BeamSpot/BeamLine.    
    
    @author Andreas.Salzburger@cern.ch
    
    */
    
namespace Reco {
                        
  class TrackToVertex : virtual public ITrackToVertex, 
        public AthAlgTool, virtual public IIncidentListener 
  {
    public:
           
      /** AlgTool like constructor */
      TrackToVertex(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~TrackToVertex();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
                  
      /** handle for incident service */
      void handle(const Incident& inc) ;

      /** Interface method for use with TrackParticle and given vertex position - AOD */
      const Trk::Perigee* perigeeAtVertex(const Rec::TrackParticle& tp, const Amg::Vector3D& gp) const;

      /** Interface method for use with xAOD::Trackparticle and given vertex position - xAOD */
      const Trk::Perigee* perigeeAtVertex(const xAOD::TrackParticle& tp, const Amg::Vector3D& gp) const;
            
      /** Interface method for use with TrackParticle and default primary vertex from TrackParticle  - AOD */
      const Trk::Perigee* perigeeAtVertex(const Rec::TrackParticle& tp) const;
          
      /** Interface method for use with TrackParticle and default primary vertex from TrackParticle  - xAOD */
      const Trk::Perigee* perigeeAtVertex(const xAOD::TrackParticle& tp) const;          
          
      /** Interface method for use with Track and given vertex position - ESD */
      const Trk::Perigee* perigeeAtVertex(const Trk::Track& trk, const Amg::Vector3D& gp) const;
                                             
      /** Interface method for use with TrackParticle and the beamspot from the BeamSpotSvc - AOD*/
      const Trk::Perigee* perigeeAtBeamspot(const Rec::TrackParticle& tp) const;
      
      /** Interface method for use with TrackParticle and the beamspot from the BeamSpotSvc - xAOD*/
      const Trk::Perigee* perigeeAtBeamspot(const xAOD::TrackParticle& tp) const;
          
      /** Interface method for use with Track and the beamspot from the BeamSpotSvc - ESD */
      const Trk::Perigee* perigeeAtBeamspot(const Trk::Track& trk) const;   

      /** Interface method for use with Track and the beamline from the BeamSpotSvc - ESD */
      const Trk::Perigee* perigeeAtBeamline(const Trk::Track& trk) const;   
      
      /** Interface method for use with TrackParticle and the beamline from the BeamSpotSvc - AOD*/
      const Trk::TrackParameters* trackAtBeamline(const Rec::TrackParticle& tp) const;
      
      /** Interface method for use with TrackParticle and the beamline from the BeamSpotSvc - xAOD*/
      const Trk::TrackParameters* trackAtBeamline(const xAOD::TrackParticle& tp) const;
          
      /** Interface method for use with Track and the beamline from the BeamSpotSvc - ESD */
      const Trk::TrackParameters* trackAtBeamline(const Trk::Track& trk) const;      
      
      /** Interface method for use with Track and the beamline from the BeamSpotSvc - TrackParameters  */
      const Trk::TrackParameters* trackAtBeamline(const Trk::TrackParameters& tpars) const;      
                                             
   private:
     ToolHandle< Trk::IExtrapolator > m_extrapolator;            //!< ToolHandle for Extrapolator
     ServiceHandle<IBeamCondSvc>      m_beamConditionsService;   //!< SurfaceHandle for conditions services
     ServiceHandle<IIncidentSvc>      m_incidentSvc;            //!< IncidentSvc to catch begin of event and end of envent
     
     Trk::StraightLineSurface*        m_beamLine;
     
     static Amg::Vector3D             s_origin;                  //!< static origin             
  };

} // end of namespace


#endif // RECOTOOLS_TRACKTOVERTEX_H

