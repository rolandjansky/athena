/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IInDetVertexTruthMatchTool_h
#define IInDetVertexTruthMatchTool_h

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"


/** Class for vertex truth matching.
 * Match reconstructed vertices to truth level interactions vertices
 * through the chain: track -> particle -> genEvent -> genVertex
 * Categorize reconstructed vertices depending on their composition.
 */

class IInDetVertexTruthMatchTool : public virtual asg::IAsgTool {

ASG_TOOL_INTERFACE( IInDetVertexTruthMatchTool );

public:

//take const collection of vertices, match them, and decorate with matching info
 virtual StatusCode matchVertices( const xAOD::VertexContainer & vxContainer ) = 0;

};

#endif
