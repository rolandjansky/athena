///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VxCandidateXAODVertex.h 
// Header file for class VxCandidateXAODVertex
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef VXEDMXAODCNV_VXCANDIDATEXAODVERTEX_H
#define VXEDMXAODCNV_VXCANDIDATEXAODVERTEX_H


#include "IVxCandidateXAODVertex.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


namespace Trk {

class VxCandidateXAODVertex
  : virtual public IVxCandidateXAODVertex,
    public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 


  // Copy constructor: 

  /// Constructor with parameters: 
  VxCandidateXAODVertex( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor:
  virtual ~VxCandidateXAODVertex() = default;

  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  ///////////////////////////////////////////////////////////////////
  /**
   * Convert xAOD::Vertex to a VxCandidate. Return object is own by the clients.
   */
  virtual StatusCode createVxCandidate(
    const xAOD::Vertex& xAODVx,
    Trk::VxCandidate*& vxVertex) const override;

  /**
   * Convert a VxCandidate to a VxVertex. Return object is own by the clients.
   */
  virtual StatusCode createXAODVertex(const Trk::VxCandidate& vxVertex,
                                      xAOD::Vertex*& xAODVx) const override;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private: 

  /// Default constructor: 
  VxCandidateXAODVertex();

  ToolHandle<Trk::IVertexLinearizedTrackFactory> m_LinearizedTrackFactory;

}; 

} // end namespace Trk

#endif //> !VXEDMXAODCNV_VXCANDIDATEXAODVERTEX_H
