///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IVxCandidateXAODVertex.h 
// Header file for class IVxCandidateXAODVertex
// Author: S.Pagan Griso <simone.pagan.griso@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRKVXEDMCVN_IVXCANDIDATEXAODVERTEX_H
#define TRKVXEDMCVN_IVXCANDIDATEXAODVERTEX_H

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
namespace Trk {
  class VxCandidate;
}
class VxContainer;


namespace Trk {
static const InterfaceID IID_IVxCandidateXAODVertex("IVxCandidateXAODVertex", 1, 0);

/** 
 * @class Trk::IVxCandidateXAODVertex 
 *
 * Interface class to hold conversion of xAOD EDM to/from vertex internal EDM.
 * 
 * @author: S. Pagan Griso <simone.pagan.griso@cern.ch>
 */


class IVxCandidateXAODVertex
  : virtual public ::IAlgTool
{ 

 public: 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID() { return IID_IVxCandidateXAODVertex; };


  /*
   * Virtual destructor.
   */
  virtual ~IVxCandidateXAODVertex(){};

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  /**
   * Convert xAOD::Vertex to a VxCandidate. 
   * @param xAODVx input object
   * @param vxVertex return object, owned by the clients.
   */
  virtual StatusCode createVxCandidate(const xAOD::Vertex &xAODVx, Trk::VxCandidate* &vxVertex) = 0;

  /**
   * Convert a VxCandidate to a VxVertex. Return object is own by the clients.
   * If a null pointer is given as output, then a new object is created with its own AuxStore.
   * Otherwise pass a new xAOD::Vertex object already attached to an aux store (preferred).
   * @param vxVertex input object
   * @param xAODVx return object, owned by the clients
   */
  virtual StatusCode createXAODVertex(const Trk::VxCandidate &vxVertex, xAOD::Vertex* &xAODVx) = 0;

  /**
   * Convert a xAOD::VertexContainer to a VxContainer
   * @param xAODVxContainer input container of xAOD::Vertex objects
   * @param vxContainer output container of Trk::VxCandidate objects
   * @param vxCandidateContainerName register output to storegate if different from null string
   */
  virtual StatusCode createVxContainer(const xAOD::VertexContainer &xAODVxContainer, VxContainer* &vxContainer, std::string vxCandidateContainerName="") = 0;

  /** Convert a VxContainer to a xAOD::VertexContainer with its own aux store.
   * @param vxContainer input @c VxContainer object
   * @param xAODVxContainer output @c xAOD::VertexContainer object
   * @param xAODAuxContainer output @c xAOD::VertexAuxContainer object
   * @param xAODContainerName output container key, will be used to store it in StoreGate if provided
   */
  virtual StatusCode createXAODVertexContainer(const VxContainer &vxContainer, 
					       xAOD::VertexContainer* &xAODVxContainer, xAOD::VertexAuxContainer* &xAODVxAuxContainer, 
					       std::string xAODContainerName="") = 0;

}; 

} //namespace Trk
#endif //> !TRKVXEDMCVN_IVXCANDIDATEXAODVERTEX_H
