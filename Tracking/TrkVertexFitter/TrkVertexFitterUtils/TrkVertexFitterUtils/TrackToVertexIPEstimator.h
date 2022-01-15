/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERUTILS_TRACKTOVERTEXIPESTIMATOR_H
#define TRKVERTEXFITTERUTILS_TRACKTOVERTEXIPESTIMATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexFitterInterfaces/IVertexUpdator.h"

namespace Trk
{

 class Track;
 class TrackParticleBase;


 
/**
 * A class estimating the transverse impact parameter d0 and
 * its error of a given track wrt to the reconstructed vertex.
 *
 * Algorithm:  
 *
 * (a) Provided track or track particle are checked,
 * whether they do already belong to the vertex.
 * (b) In the positive case, the vertex is reduced by
 * removing the track or track particle in question.
 * (c) The trajectory state is extrapolated to the 
 * perigee with the vertex in question, the d0 
 * and its error is estimated. The error on the vertex
 * position is included in this calculation.
 *
 * Kirill.Prokofiev@cern.ch February 2008
 *
 * --------------------------------------
 * Changes:
 *
 * Giacinto.Piacquadio@physik.uni-freiburg.de (update of 11.September 2008)
 *
 * In addition the tool is now able to provide the 
 * impact parameter z0 and its error.
 *
 * At the same time an option can be passed not to remove 
 * the track from the vertex (non-default)
 *
 * In addition, the lifetime sign of the Impact Parameter 
 * given a lifetime direction of flight is also provided 
 * with 3 methods: 2D, Z and full 3D.
 *
 * --------------------------------------
 * Johanna Fleckner (03.12.2009)
 * allow for estimation of IP parameter even if vertex doesn't contain any tracks
 * as can happen in BeamSpot or early running when PV is not reconstructed
 * degrade some INFO/WARNINGS to DEBUG
 *
 * --------------------------------------
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 * EDM Migration to xAOD - Remove methods involving Trk::VxCandidate and Trk::RecVertex
 *
 */ 
 
 
  class TrackToVertexIPEstimator : public AthAlgTool, virtual public ITrackToVertexIPEstimator
 {
 
  public:
 

 /**
  * Default Athena interface methods
  */
    virtual StatusCode initialize() override;

 
 /**
  * Default Athena interface constructor and destructor 
  */  
    TrackToVertexIPEstimator(const std::string& t, const std::string& n, const IInterface*  p);
    
   ~TrackToVertexIPEstimator();
  
/**
 * Estimate methods returning a d0 and its calculated sigma.
 * All  methods do check whether a track is actually fitted to a vertex
 */  
   virtual const  ImpactParametersAndSigma * estimate(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx, bool doRemoval) const override;
   virtual const  ImpactParametersAndSigma * estimate(const TrackParameters * track, const xAOD::Vertex * vtx, bool doRemoval) const override;

   virtual const  ImpactParametersAndSigma * estimate(const xAOD::TrackParticle * track,const xAOD::TrackParticle * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const override;
   virtual const  ImpactParametersAndSigma * estimate(const TrackParameters * track, const TrackParameters * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const override;

   virtual const xAOD::Vertex *   getUnbiasedVertex(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx ) const override;
   virtual const xAOD::Vertex *   getUnbiasedVertex(const TrackParameters * track, const xAOD::Vertex * vtx ) const override;
  

   virtual const ImpactParametersAndSigma  * estimate(const xAOD::TrackParticle * track, const xAOD::Vertex* vtx)const override;
   virtual const ImpactParametersAndSigma  * estimate(const TrackParameters * track, const xAOD::Vertex* vtx)const override;
   

   /* Methods which provide the lifetime Sign for the Impact Parameter (2D, Z, 3D)  */
   virtual
   double get3DLifetimeSignOfTrack(const TrackParameters & track,
                                   const Amg::Vector3D & jetDirection,
                                   const xAOD::Vertex & primaryVertex) const override;
   virtual
   double get3DLifetimeSignOfTrack(const TrackParameters & track,
                                   const CLHEP::Hep3Vector & jetDirection,
                                   const xAOD::Vertex & primaryVertex) const override;

   virtual
   double get2DLifetimeSignOfTrack(const TrackParameters & track,
                                   const Amg::Vector3D & jetDirection,
                                   const xAOD::Vertex & primaryVertex) const override;
   virtual
   double get2DLifetimeSignOfTrack(const TrackParameters & track,
                                   const CLHEP::Hep3Vector & jetDirection,
                                   const xAOD::Vertex & primaryVertex) const override;

   virtual
   double getZLifetimeSignOfTrack(const TrackParameters & track,
                                  const Amg::Vector3D & jetDirection,
                                  const xAOD::Vertex & primaryVertex) const override;
   virtual
   double getZLifetimeSignOfTrack(const TrackParameters & track,
                                  const CLHEP::Hep3Vector & jetDirection,
                                  const xAOD::Vertex & primaryVertex) const override;




  private:

/**
 * A method calculating the do and its  error.
 * At this stage the trajectory is removed from the 
 * vertex.
 */
   const ImpactParametersAndSigma  * calculate(const TrackParameters * track, const xAOD::Vertex& vtx) const;

   ToolHandle< Trk::IExtrapolator >  m_extrapolator { this, "Extrapolator", "Trk::Extrapolator" }; 
   ToolHandle< Trk::IVertexUpdator > m_Updator {this, "VertexUpdator", "Trk::KalmanVertexUpdator"};
   ToolHandle< Trk::IVertexLinearizedTrackFactory > m_linFactory {this, "LinearizedTrackFactory", "Trk::FullLinearizedTrackFactory"};

 };//end of class definitions


}//end of namespace definitions

#endif
