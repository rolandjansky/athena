// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Example implementation class for the JetConstsituentModSequence sequencer.
// A daughter class of JetConstituentModifierBase. 
//
// Author: Michael Nelson, University of Oxford, UK
//


#ifndef CALOCLUSTERCONSTITUENTSORIGIN_H
#define CALOCLUSTERCONSTITUENTSORIGIN_H

#include "AsgTools/AsgTool.h" 
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

class CaloClusterConstituentsOrigin: public JetConstituentModifierBase {
  ASG_TOOL_CLASS(CaloClusterConstituentsOrigin, IJetConstituentModifier) 
  
  public:
  
  CaloClusterConstituentsOrigin(const std::string & name); // MEN: constructor 

  // Check that the configuration is reasonable
  StatusCode initialize();
  
  private:

  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 
  StatusCode correctToOriginVtx(xAOD::CaloClusterContainer& cont, const xAOD::Vertex& vert) const;

  std::string m_vertexContName = "PrimaryVertices";
  
};

#endif
