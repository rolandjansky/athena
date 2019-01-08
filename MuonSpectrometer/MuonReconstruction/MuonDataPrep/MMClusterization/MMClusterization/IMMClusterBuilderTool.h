/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IMMClusterBuilderTool_h
#define IMMClusterBuilderTool_h

//
// Interface class for MM clustering
//
#include <vector>
#include "GaudiKernel/IAlgTool.h"

namespace Muon {
  class MMPrepData;
}

static const InterfaceID IID_IMMClusterBuilderTool("Muon::IMMClusterBuilderTool", 1, 0);

namespace Muon {

  class IMMClusterBuilderTool : virtual public IAlgTool {
    
  public:    // static methods
    
    static const InterfaceID& interfaceID()  { return IID_IMMClusterBuilderTool; } 
    
  public:    // interface methods
    
    virtual StatusCode getClusters(std::vector<Muon::MMPrepData>& stripsVect, 
				   std::vector<Muon::MMPrepData>& clustersVect)=0;
    
  };
}

#endif
