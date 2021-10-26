/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IInDetSecVertexTruthMatchTool_h
#define IInDetSecVertexTruthMatchTool_h

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthVertexContainer.h"


/** Class for vertex truth matching.
 * Match reconstructed vertices to truth level interactions vertices
 * through the chain: track -> particle -> genEvent -> genVertex
 * Categorize reconstructed vertices depending on their composition.
 */

class IInDetSecVertexTruthMatchTool : public virtual asg::IAsgTool {

ASG_TOOL_INTERFACE( IInDetSecVertexTruthMatchTool )

public:

//take const collection of vertices, match them, and decorate with matching info
 virtual StatusCode matchVertices( const xAOD::VertexContainer & vtxContainer,
                                   const xAOD::TruthVertexContainer & truthVtxContainer ) const = 0;

 virtual StatusCode labelTruthVertices( const xAOD::TruthVertexContainer & truthVtxContainer ) const = 0;

};

#endif
