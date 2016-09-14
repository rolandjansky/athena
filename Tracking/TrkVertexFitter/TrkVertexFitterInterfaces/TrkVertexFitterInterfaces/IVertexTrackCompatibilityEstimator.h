/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexTrackCompatibilityEstimator, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACES_IVERTEXTRACKCOMPATIBILITYESTIMATOR_H
#define TRKVERTEXFITTERINTERFACES_IVERTEXTRACKCOMPATIBILITYESTIMATOR_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkParameters/TrackParameters.h"

  /**
   * @class Trk::IVertexTrackCompatibilityEstimator
   *
   * An Abstract base class for estimators, calculating
   * the compatibility of a track and a vertex. For the
   * actual compatibility measure (usually chi2) see
   * concrete implementation.
   *
   * Estimate methods: the void method is used only inside the
   * vertex fitter. It updates the chi2 of the LinearizedTrack  
   * contained in the Trk::VxTrackAtVertex with the compatibility estimate
   * The float method returns directly the measure of compatibility 
   * between track and vertex. Methods of other types may be added on
   * request.
   *
   * @author Giacinto.Piacquadio@physik.uni-freiburg.de
   * @author Kirill.Prokofiev@cern.ch 
   *
   * -------------------------------------------------------------
   * Changes:
   *
   * David Shope <david.richard.shope@cern.ch> (2016-03-18)
   * EDM Migration to xAOD - move Trk::Vertex to Amg::Vector3D
   *
   */

namespace Trk
{

  static const InterfaceID IID_IVertexTrackCompatibilityEstimator("IVertexTrackCompatibilityEstimator", 1, 0);


  class VxTrackAtVertex;

  class IVertexTrackCompatibilityEstimator : virtual public IAlgTool 
  {

    public:
     /** 
      * Virtual destructor 
      */
       virtual ~IVertexTrackCompatibilityEstimator(){};

     /** 
      * AlgTool interface methods 
      */
       static const InterfaceID& interfaceID() { return IID_IVertexTrackCompatibilityEstimator; };

     /**
      * An estimate method, updating the Trk::VxTrackAtVertex internally
      */
       virtual void estimate(VxTrackAtVertex &,const Amg::Vector3D & vertex)=0;
       
     /**
      * An estimate method, returning the compatibility value directly
      */    
       virtual float compatibility(VxTrackAtVertex &,const Amg::Vector3D & vertex)=0;

  };
}

#endif
