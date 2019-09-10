/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ISTgcClusterBuilderTool_h
#define ISTgcClusterBuilderTool_h

//
// Interface class for STgc clustering
//
#include <vector>
#include "GaudiKernel/IAlgTool.h"

namespace Muon {
  class sTgcPrepData;
}

class IdentifierHash;

static const InterfaceID IID_ISTgcClusterBuilderTool("Muon::ISTgcClusterBuilderTool", 1, 0);

namespace Muon {
  class ISTgcClusterBuilderTool : virtual public IAlgTool {
    
  public:    // static methods
    
    static const InterfaceID& interfaceID()  { return IID_ISTgcClusterBuilderTool; } 
  
  public:    // interface methods
    
    //
    // build clusters having as input the hashId of the collection, the
    // resolution of the single channel, and the vector of firing strips
    //
    virtual StatusCode getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, 
				   std::vector<Muon::sTgcPrepData*>& clustersVect)=0;
    
  };
}

#endif
