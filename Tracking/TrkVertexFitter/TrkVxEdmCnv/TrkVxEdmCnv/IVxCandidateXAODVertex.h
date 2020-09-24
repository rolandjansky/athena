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
  virtual ~IVxCandidateXAODVertex()=default;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  ///////////////////////////////////////////////////////////////////
  /**
   * Convert xAOD::Vertex to a VxCandidate.
   * @param xAODVx input object
   * @param vxVertex return object, owned by the clients.
   */
  virtual StatusCode createVxCandidate(const xAOD::Vertex& xAODVx,
                                       Trk::VxCandidate*& vxVertex) const = 0;

  /**
   * Convert a VxCandidate to a VxVertex. Return object is own by the clients.
   * If a null pointer is given as output, then a new object is created with its
   * own AuxStore. Otherwise pass a new xAOD::Vertex object already attached to
   * an aux store (preferred).
   * @param vxVertex input object
   * @param xAODVx return object, owned by the clients
   */
  virtual StatusCode createXAODVertex(const Trk::VxCandidate& vxVertex,
                                      xAOD::Vertex*& xAODVx) const = 0;
};

} //namespace Trk
#endif //> !TRKVXEDMCVN_IVXCANDIDATEXAODVERTEX_H
