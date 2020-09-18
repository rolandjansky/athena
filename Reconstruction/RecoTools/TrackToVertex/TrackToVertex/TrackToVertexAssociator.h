///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TrackToVertexAssociator.h 
// Header file for class TrackToVertexAssociator
// Author: Andreas.Salzburger -at- cern.ch 
/////////////////////////////////////////////////////////////////// 
#ifndef TRACKTOVERTEX_TRACKTOVERTEXASSOCIATOR_H
#define TRACKTOVERTEX_TRACKTOVERTEXASSOCIATOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

// ITrackToVertex includes
#include "ITrackToVertex/ITrackToVertexAssociator.h"

// Tracking includes
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration
class StoreGateSvc;
class IIncidentSvc;
class VxContainer;

namespace Trk {
    class Track;
    class TrackParticleBase;
    class StraightLineSurface;
    class StraightLineSurface;
}


namespace Reco {
    
    class ATLAS_NOT_THREAD_SAFE TrackToVertexAssociator
  : virtual public ITrackToVertexAssociator,
            public AthAlgTool, virtual public IIncidentListener {
 
   /////////////////////////////////////////////////////////////////// 
   // Public methods: 
   /////////////////////////////////////////////////////////////////// 
  public: 
 
   // Copy constructor: 
 
   /// Constructor with parameters: 
   TrackToVertexAssociator( const std::string& type,
 	     const std::string& name, 
 	     const IInterface* parent );
 
   /// Destructor: 
   virtual ~TrackToVertexAssociator(); 
 
   // Athena algtool's Hooks
   StatusCode  initialize();
   StatusCode  finalize();
 
   /** handle for incident service */
   void handle(const Incident& inc) ;
      
   /** Interface method for use with TrackParticle  - AOD */
   const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::TrackParticleBase& part) const;
   
   /** Interface method for use with Track  - ESD */
   const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::Track& trk) const;
                                             
   /** Interface method for use with ParametersBase - parameters */
   const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::TrackParameters& pbase) const;

   /** Interface method for use with neutral TPs */
   const std::vector<TrackToVertexAssociation>* associateTrack ATLAS_NOT_THREAD_SAFE (const Trk::NeutralParameters& pars) const;
  private: 

   template < class T > const std::vector<TrackToVertexAssociation>* runAssociation ATLAS_NOT_THREAD_SAFE (const T& input) const;
 
   // Default constructor: 
   TrackToVertexAssociator();

   // private methods 
   StatusCode updateCache ATLAS_NOT_THREAD_SAFE () const;
   StatusCode cleanCache ATLAS_NOT_THREAD_SAFE  () const;
      
   // Services
   ServiceHandle<StoreGateSvc>                   m_storeGate;     //!< Event store
   SG::ReadCondHandleKey<InDet::BeamSpotData>    m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };   //!< BeamCondSvc to get the beam parameters
   ServiceHandle<IIncidentSvc>                   m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent
                                                 
   // Tools                                      
   ToolHandle<Trk::IExtrapolator>                m_extrapolator;  //!< Extrapolation engine for estimation of the impact

   std::string                                   m_primaryVertexContainer;   
   mutable const VxContainer*                    m_primaryVertices;
   mutable std::vector<const Trk::VxCandidate*>  m_primaryVerticesWoDummy; 
   mutable std::vector<Amg::Vector3D>      m_primaryVerticesBLFrame; 
                                         
   mutable Trk::StraightLineSurface*             m_beamLine;
   bool                                          m_particleHypo;
   double                                        m_cut;
   
 
 }; 
  
 /////////////////////////////////////////////////////////////////// 
 // Inline methods: 
 /////////////////////////////////////////////////////////////////// 
 
 /** Interface method for use with ParametersBase - work horse */
 template < class T > const std::vector<TrackToVertexAssociation>* TrackToVertexAssociator::runAssociation(const T& tinput) const
 {
     if (!m_primaryVerticesBLFrame.size() && updateCache().isFailure()){
         ATH_MSG_DEBUG("No primary vertices for this event, return 0.");
         return 0;
     }
     // extrapolate to the beam line, can we use a data pool for this ?
     const Trk::TrackParameters* ataLine = m_extrapolator->extrapolate(tinput,*m_beamLine,Trk::anyDirection,(Trk::ParticleHypothesis)m_particleHypo);
     if (!ataLine) {
         ATH_MSG_DEBUG("Extrapolation to beamline did not succeed, return 0.");
         return 0;
     }

     Trk::SharedObject<const Amg::Vector2D> localImpactParameter(new Amg::Vector2D(ataLine->localPosition()));
     Amg::MatrixX* cov = 0;
     if (ataLine->covariance()){
         // d0, z0
         double covD0 = (*ataLine->covariance())(Trk::d0,Trk::d0);
	 double covZ0 = (*ataLine->covariance())(Trk::z0,Trk::z0);
         // covariance matrix 
         cov = new Amg::MatrixX(2,2);
	 cov->setZero();
         (*cov)(Trk::d0,Trk::d0) = covD0;
         (*cov)(Trk::z0,Trk::z0) = covZ0;
     }
     Trk::SharedObject<const Amg::MatrixX> localCovariance(cov);
     // create the return vector and reserve the maximum you could possibly need
     std::vector< Reco::TrackToVertexAssociation>* tvaVector = new std::vector< Reco::TrackToVertexAssociation >();
     tvaVector->reserve(m_primaryVerticesBLFrame.size());
     // loop and build the associations
     std::vector<Amg::Vector3D>::const_iterator vBLIter         = m_primaryVerticesBLFrame.begin();
     std::vector<Amg::Vector3D>::const_iterator vBLIterEnd      = m_primaryVerticesBLFrame.end();
     std::vector<const Trk::VxCandidate*>::const_iterator vIter       = m_primaryVerticesWoDummy.begin();
     std::vector<const Trk::VxCandidate*>::const_iterator vIterEnd    = m_primaryVerticesWoDummy.end();
     // loop over and build the TrackToVertex associations
     for ( ; vBLIter!= vBLIterEnd && vIter != vIterEnd; ++vBLIter, ++vIter )
         tvaVector->push_back(Reco::TrackToVertexAssociation(*vIter,*vBLIter,localImpactParameter,localCovariance));
     // memory cleanup
     delete ataLine; 
     // sort the vector before returning, using operator< defined for TrackToVertexAssociation
     std::sort(tvaVector->begin(),tvaVector->end());
     // return what you have
     return tvaVector;
 }
 
 
}

#endif //> !TRACKTOVERTEX_TRACKTOVERTEXASSOCIATOR_H
