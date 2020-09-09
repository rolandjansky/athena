/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackToVertexAssociator.h, (c) ATLAS Detector software 2010
///////////////////////////////////////////////////////////////////

#ifndef TRACKTOVERTEX_ITRACKTOVERTEXASSOCIATOR_H
#define TRACKTOVERTEX_ITRACKTOVERTEXASSOCIATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkDetDescrUtils/SharedObject.h"

// (Track/Neutral)Parameters
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
 
namespace Trk {
 class Track;
 class TrackParticleBase;
 class VxCandidate;

}

namespace Reco {

  /**
    @class TrackToVertexAssociation
      -- this is a container class with reference counting for the private members
      NEVER DELETE A RETURN OBJECT !
  */
  class TrackToVertexAssociation {
     public :
       /** Constructor */
       TrackToVertexAssociation(const Trk::VxCandidate* vertex,
                                const Amg::Vector3D& vertexPositionBLFrame,
                                Trk::SharedObject<const Amg::Vector2D>     impactParameterBLFrame,
                                Trk::SharedObject<const Amg::MatrixX>  covarianceMatrixBlFrame
                                ) :
            m_vertex(vertex),
            m_impactParameterBLFrame(impactParameterBLFrame),
            m_covarianceMatrixBlFrame(covarianceMatrixBlFrame)
        {
            m_longitudinalDistance = (*impactParameterBLFrame)[Trk::z0] - vertexPositionBLFrame.z();
            m_transverseDistance   = (*impactParameterBLFrame)[Trk::d0] - vertexPositionBLFrame.perp(); 
        }
        
        //** Destructor */
        ~TrackToVertexAssociation(){}
        
        // Copy constructor 
        TrackToVertexAssociation(const TrackToVertexAssociation& ttva) :
             m_vertex(ttva.m_vertex),
             m_impactParameterBLFrame(ttva.m_impactParameterBLFrame),
             m_covarianceMatrixBlFrame(ttva.m_covarianceMatrixBlFrame),
             m_longitudinalDistance(ttva.m_longitudinalDistance),
             m_transverseDistance(ttva.m_transverseDistance) 
        {}
        
        // Assignment operator
        TrackToVertexAssociation& operator=(const TrackToVertexAssociation& ttva ){
            if (&ttva != this){
                m_vertex = ttva.m_vertex;
                m_impactParameterBLFrame  = ttva.m_impactParameterBLFrame;
                m_covarianceMatrixBlFrame = ttva.m_covarianceMatrixBlFrame; 
                m_longitudinalDistance    = ttva.m_longitudinalDistance;
                m_transverseDistance      = ttva.m_transverseDistance;
            }
            return (*this);
        }
      
       // define the smaller operator for this object
       bool operator<(const TrackToVertexAssociation& b) const 
           { return m_longitudinalDistance*m_longitudinalDistance < b.longitudinalDistance()*b.longitudinalDistance(); }
      
        // Return the vertex -- original frame
        const Trk::VxCandidate* vertex() const { return m_vertex; }
      
        // Return the local position
        const Amg::Vector2D* impactParameterBeamLineFrame() const { return (m_impactParameterBLFrame.get()); };

        // Return the Covariance Matrix*
        const Amg::MatrixX* covarianceMatrixBeamLineFrame() const { return (m_covarianceMatrixBlFrame.get()); }

        // distances (1) : transverse
        double transverseDistance() const { return m_transverseDistance; }

        // distances (2) : longitudinal
        double longitudinalDistance() const { return m_longitudinalDistance; }

      
     private : 
      // Default constructor is private
      TrackToVertexAssociation() {};
     
      // the vertices in original frame in beam-line frame
      const Trk::VxCandidate*                   m_vertex;
      // the local position and error to the BL, to do : add Covariance matrix
      Trk::SharedObject<const Amg::Vector2D>     m_impactParameterBLFrame;
      Trk::SharedObject<const Amg::MatrixX>  m_covarianceMatrixBlFrame;
      // for the sorting
      double m_longitudinalDistance;
      double m_transverseDistance;      
  };


  static const InterfaceID IID_ITrackToVertexAssociator("ITrackToVertexAssociator", 1, 0);
  
  /** 
   @class ITrackToVertexAssociator
          
   @author Andreas.Salzburger@cern.ch
   */
      
      
  class ITrackToVertexAssociator : virtual public IAlgTool {
     
     public:
       /** Virtual destructor */
       virtual ~ITrackToVertexAssociator(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITrackToVertexAssociator; };
      
       /** Interface method for use with TrackParticle  - AOD */
       virtual const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::TrackParticleBase& part) const = 0;
       
       /** Interface method for use with Track  - ESD */
       virtual const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::Track& trk) const = 0;

       /** Interface method for use with charged TPs - work horse */
       virtual const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::TrackParameters& pars) const = 0;

       /** Interface method for use with neutral TPs */
       virtual const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::NeutralParameters& pars) const = 0;                                                      
  };

} 

#endif // TRACKTOVERTEX_ITRACKTOVERTEXASSOCIATOR_H

