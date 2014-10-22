/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*- header for IJetFitterVariablesFactory
#ifndef BTAGTOOL_IMSVVARIABLESFACTORY_C
#define BTAGTOOL_IMSVVARIABLESFACTORY_C

/******************************************************
    @class  MSVVariableFactory
    Package : JetTagTools 
    Created : Sept 2014
    DESCRIPTION:
    This class is a "variable factory". It generates all 
    the variables per vertex in MSV.
********************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"
#include "xAODBTagging/BTagging.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include <string> 

namespace Trk {
  class VxSecVKalVertexInfo;
}

namespace Analysis {

  static const InterfaceID IID_IMSVVariablesFactory
  ("Analysis::IMSVVariablesFactory", 1, 0);

  
  class IMSVVariablesFactory : virtual public IAlgTool  {
    
  public:
    
    virtual ~IMSVVariablesFactory(){};
    
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    //virtual void setOrigin(const xAOD::Vertex* priVtx) = 0;
    
    virtual StatusCode fillMSVVariables(const xAOD::Jet &, xAOD::BTagging* BTag, const Trk::VxSecVKalVertexInfo* myInfoVKal, xAOD::VertexContainer* btagVertex, const xAOD::Vertex& PV, std::string basename)const  = 0;


    static const InterfaceID& interfaceID() { return IID_IMSVVariablesFactory; };

  }; 

 
  
}//end Analysis namespace

#endif
