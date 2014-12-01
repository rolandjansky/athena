/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File: TileAssocFillerTool.h
 * Author: Carlos.Solans@cern.ch
 * July 2014
 * Implementation of a VoidBlockFillerTool
 * PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
 */

#ifndef TILEASSOCFILLERTOOL_H
#define TILEASSOCFILLERTOOL_H

#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <string>

typedef std::vector<std::vector<int> > ASSOCCONTAINER;
#define ASSOCCONTAINER_CONSTRUCTOR(size) ASSOCCONTAINER( size ,std::vector<int>(0))

namespace D3PD{

  class TileAssocFillerTool:public BlockFillerTool<void>{
  public:

    TileAssocFillerTool(const std::string& type,const std::string& name, const IInterface* parent);
    
    StatusCode initialize();
    virtual StatusCode book();
    virtual StatusCode fill();
    
  private:

    std::string m_branchName; 
    std::string m_containerName; 
    ASSOCCONTAINER* m_container;
    StoreGateSvc* m_storeGate;
  
  };
  
} 

#endif

