/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERINTERFACES_ITRACKTOVERTEXIPESTIMATOR_H
#define TRKVERTEXFITTERINTERFACES_ITRACKTOVERTEXIPESTIMATOR_H

#include "GaudiKernel/IAlgTool.h" 
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

/**
 * An abstract base class  for algorithms estimating the transverse impact parameter d0 and
 * its error of a given track wrt to the reconstructed vertex.
 * 
 * K.Prokofiev, C.Ruwiedel, June 2009.
 *
 * Changes: G. Piacquadio, July 2009, add lifetime signs for b-tagging
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *   EDM Migration to xAOD - Remove methods involving Trk::VxCandidate and Trk::RecVertex 
 *
 */ 

namespace Trk
{


 struct ImpactParametersAndSigma 
 {
   double IPd0;
   double IPz0;
   double IPz0SinTheta;
   double sigmad0;
   double sigmaz0;
   double sigmaz0SinTheta;
   double PVsigmad0;
   double PVsigmaz0;
   double PVsigmaz0SinTheta;
   
 };
   
 
 static const InterfaceID IID_ITrackToVertexCompatibilityEstimator("ITrackToVertexIPEstimator", 1, 0);
 
 class ITrackToVertexIPEstimator: virtual public IAlgTool
 {
 
  public:
 
   virtual ~ITrackToVertexIPEstimator(){};
 
   static const InterfaceID& interfaceID() { return IID_ITrackToVertexCompatibilityEstimator; };
 
/**
 * Estimate methods returning a d0 and its calculated sigma.
 * All  methods do check whether a track is actually fitted to a vertex
 */  
   virtual const  ImpactParametersAndSigma * estimate(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx, bool doRemoval) const = 0;
   virtual const  ImpactParametersAndSigma * estimate(const Trk::TrackParameters * track, const xAOD::Vertex * vtx, bool doRemoval) const = 0;


   virtual const  ImpactParametersAndSigma * estimate(const xAOD::TrackParticle * track, const xAOD::TrackParticle * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const = 0;
   virtual const  ImpactParametersAndSigma * estimate(const Trk::TrackParameters * track, const Trk::TrackParameters * newtrack, const xAOD::Vertex * vtx, bool doRemoval) const = 0;


   virtual const xAOD::Vertex *   getUnbiasedVertex(const xAOD::TrackParticle * track, const xAOD::Vertex * vtx ) const = 0;
   virtual const xAOD::Vertex * getUnbiasedVertex(const Trk::TrackParameters * track, const xAOD::Vertex * vtx ) const =0;

   virtual const ImpactParametersAndSigma  * estimate(const xAOD::TrackParticle * track, const xAOD::Vertex* vtx)const =0;
   virtual const ImpactParametersAndSigma  * estimate(const Trk::TrackParameters * track, const xAOD::Vertex* vtx)const =0;


  virtual double get3DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                          const Amg::Vector3D & jetMomentum,
                                          const xAOD::Vertex & primaryVertex) const =0;
  virtual double get3DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                          const CLHEP::Hep3Vector & jetMomentum,
                                          const xAOD::Vertex & primaryVertex) const =0;
   

  virtual double get2DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                          const Amg::Vector3D & jetMomentum,
                                          const xAOD::Vertex & primaryVertex) const=0;
  virtual double get2DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                          const CLHEP::Hep3Vector & jetMomentum,
                                          const xAOD::Vertex & primaryVertex) const=0;
   
  virtual double getZLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                         const Amg::Vector3D & jetMomentum,
                                         const xAOD::Vertex & primaryVertex) const = 0;
  virtual double getZLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                         const CLHEP::Hep3Vector & jetMomentum,
                                         const xAOD::Vertex & primaryVertex) const = 0;

   

 };//end of class definitions

}
#endif
