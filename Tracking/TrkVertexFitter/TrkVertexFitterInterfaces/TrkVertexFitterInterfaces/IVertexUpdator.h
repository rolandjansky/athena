/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IVertexUpdator, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXFITTERINTERFACE_IVERTEXUPDATOR_H
#define TRKVERTEXFITTERINTERFACE_IVERTEXUPDATOR_H

#include "GaudiKernel/IAlgTool.h"

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
  */


namespace Trk
{

  static const InterfaceID IID_IVertexUpdator("IVertexUpdator", 1, 0);

  class VxCandidate;
  class VxTrackAtVertex;
  class RecVertex;
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
     
      /**
       * Add method: adds one track to a vertex
       */
       virtual VxCandidate * add(VxCandidate &,  VxTrackAtVertex &) const =0;

      /**
       * Remove method: removes one track from a vertex
       */
       virtual VxCandidate * remove(VxCandidate &,  VxTrackAtVertex &) const =0; 
   
     /**
      * Position update method. Required for smoothed chi2 calculation  
      * Updates a position or given RecVertex with the information from 
      * one track. 
      */
       virtual RecVertex positionUpdate (const RecVertex& vtx, const LinearizedTrack * trk, double trackWeight, int sign) const = 0; 
        
  };
}
#endif
