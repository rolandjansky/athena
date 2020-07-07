/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexType.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_VERTEXTYPE_H
#define TRKEVENTPRIMITIVES_VERTEXTYPE_H

namespace Trk {

 /** @enum VertexType
    
     This file defines the enums in the Trk namespace for the different vertex types.
     The VxCandidate and the TrackParticleBase class store such an enum to signal
     which vertex it is or from which vertex it comes from (in the case of the
     TrackParticleBase object).
   
    @author Andreas.Wildauer@cern.ch
   */

enum VertexType
{
  NoVtx = 0,         //!< Dummy vertex, TrackParticle was not used in vertex fit
  PriVtx = 1,        //!< Primary Vertex
  SecVtx = 2,        //!< Secondary Vertex
  PileUp = 3,        //!< Pile Up Vertex
  ConvVtx = 4,       //!< Converstion Vertex
  V0Vtx = 5,         //!< Vertex from V0 Decay
  KinkVtx = 6,       //!< Kink Vertex
  V0Lambda = 7,      //!< Temporary addition for V0 Lambda
  V0LambdaBar = 8,   //!< Temporary addition for V0 LambdaBar
  V0KShort = 9,      //!< Temporary addition for KShort
  NotSpecified = -99 //!< this is the default
};
} // end of namespace

#endif // TRKEVENTPRIMITIVES_VERTEXTYPE_H

