/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexUpdator, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXUPDATOR_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXUPDATOR_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/Vertex.h"

 /**
  * @class Trk::IVertexUpdator
  *
  * An abstract base class for the vertex updators.
  * Adds or removes a single Trk::VxTrackAtVertex to
  * or from a Trk::VxCandidate. The new Trk::VxCandidate
  * is returned at the end of this operation. The initial
  * one is not changed.
  *
  * @authors   Kirill.Prokofiev@cern.ch, Giacinto.Piacquadio@physik.uni-freiburg.de
  * 
  * ----------------------------------------------------
  * Changes:
  *
  * David Shope <david.richard.shope@cern.ch> (2016-04-19)
  *
  * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  *                         from Trk::RecVertex   to xAOD::Vertex,
  *                         from Trk::Vertex      to Amg::Vector3D
  */


namespace Trk
{

  static const InterfaceID IID_IVertexUpdator("IVertexUpdator", 1, 0);

  class VxTrackAtVertex;
  class LinearizedTrack;

  class IVertexUpdator : virtual public IAlgTool {

     public:
       /** 
        * Virtual destructor 
        */
       virtual ~IVertexUpdator(){};

       /**
        * AlgTool interface methods 
        */
       static const InterfaceID& interfaceID() { return IID_IVertexUpdator; };
     
       // Slim struct used to transport the (temporary) result of positionUpdate. 
       // Significantly faster than using an xAOD::Vertex for this purpose. 
       struct positionUpdateOutcome{
          Amg::Vector3D position;       // vertex position
          AmgSymMatrix(3) covariancePosition;   // position covariance
          AmgSymMatrix(5) trkParametersWeight;  // inverse track parameter covariance, 
                                                // cached to avoid re-calculation
       }; 

       // operating mode for positionUpdate. 
       typedef enum{
          removeTrack=-1, 
          addTrack=1 
       } updateMode;

      /**
       * Add method: adds one track to a vertex
       */
       virtual xAOD::Vertex * add(xAOD::Vertex &,  VxTrackAtVertex &) const =0;

      /**
       * Remove method: removes one track from a vertex
       */
       virtual xAOD::Vertex * remove(xAOD::Vertex &,  VxTrackAtVertex &) const = 0;

     /**
      * Position update method. Required for smoothed chi2 calculation  
      * Updates a position with the information from 
      * one track. 
      */
       virtual positionUpdateOutcome positionUpdate (const xAOD::Vertex& vtx, const LinearizedTrack * trk, double trackWeight, updateMode mode) const = 0;
       
       /**
       * Method calculating the interstep Chi2 increment.
       * Can also be called using the outcome from a prior positionUpdate call. 
       */ 
       virtual float trackParametersChi2(const xAOD::Vertex& new_vtx, const LinearizedTrack * trk) const = 0;
       virtual float trackParametersChi2(const positionUpdateOutcome & new_vtx , const LinearizedTrack * trk) const = 0;

       /** 
       * Method calculating the vertex displacement-related part of the chi2   
       * Can also be called using the outcome from a prior positionUpdate call. 
       */
       virtual float vertexPositionChi2(const xAOD::Vertex& old_vtx, const xAOD::Vertex& new_vtx) const = 0;
       virtual float vertexPositionChi2(const xAOD::Vertex& old_vtx, const positionUpdateOutcome & new_vtx) const = 0;

  };
}
#endif
