/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexLinearizedTarckFactory, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXLINEARIZEDTRACKFACTORY_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXLINEARIZEDTRACKFACTORY_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"

// For Amg
#include "GeoPrimitives/GeoPrimitives.h"

 /**
   * @class Trk::IVertexLinearizedTrackFactory
   *
   * An Abstract Base Class for the LinearizedTrackFactories.
   * Aim: Linearizes the measurement equation (dependance of track
   * parameters on the vertex position and track mometum at vertex)
   * at the vicinity of the user-provided linearization point.
   *
   * The measurement equation is linearized in the following way: 
   *
   * q_k= A_k (x_k - x_0k) + B_k (p_k - p_0k) + c_k
   *
   * where q_k are the parameters at perigee nearest to the lin point, 
   * x_k is the position of the vertex, p_k the track momentum at the vertex,
   * and c_k is the constant term of expansion. A_k and B_k are matrices
   * of derivatives, denoted hereafter as "positionJacobian" and "momentumJacobian"
   * respectively.
   *
   * Result of the calculation is stored in the object of type 
   * TrkEvent/VxVertex/PerigeeLinearizedTrack.
   * Due to the present EDM structure, the current implementation of  
   * this class supports only the perigee parametrization of the track. 
   * Extension for the arbitrary track parametrization is foreseen.
   *
   * @author Giacinto.Piacquadio@physik.uni-freiburg.de 
   * @author Kirill.Prokofiev@cern.ch
   *
   * -----------------------------------------
   * Changes:
   *
   * David Shope <david.richard.shope@cern.ch> (2016-06-01)
   *
   *   EDM Migration to xAOD - replace Trk::Vertex with Amg::Vector3D
   *
   */



namespace Trk
{

  static const InterfaceID IID_IVertexLinearizedTrackFactory("IVertexLinearizedTrackFactory", 1, 0);

  class VxTrackAtVertex;
  class LinearizedTrack;
  
  class IVertexLinearizedTrackFactory : virtual public IAlgTool 
  {

     public:

     /** 
      * Virtual destructor 
      */
       virtual ~IVertexLinearizedTrackFactory(){};

     /** 
      * AlgTool interface methods 
      */
       static const InterfaceID& interfaceID() { return IID_IVertexLinearizedTrackFactory; };

     /** 
      * Interface for Trk::VxTrackAtVertex:
      * Takes a Trk::TrackParameters from Trk::VxTrackAtVertex and a lineariztion point.
      * Produces a new perigee linearized state and stores its pointer as a private  
      * member of Trk::VxTrackAtVertex
      */
       virtual void linearize(VxTrackAtVertex & theTrack,const Amg::Vector3D & linPoint) const = 0;
      

     /**
      * Linearization method:
      * Takes a Trk::TrackParameters and a linearization point.
      * Creates a new perigee linearized track state and returns its pointer.
      */
       virtual LinearizedTrack * linearizedTrack(const TrackParameters * perigee, 
                                                 const Amg::Vector3D & linPoint) const =0;
     /**
       * Linearization method:
       * Takes a Trk::NeutralParameters and a linearization point.
       * Creates a new perigee linearized track state and returns its pointer.
       */
       virtual LinearizedTrack * linearizedTrack(const NeutralParameters * perigee,
                                                 const Amg::Vector3D & linPoint) const =0;

  };

}//end of namespace definitions

#endif
