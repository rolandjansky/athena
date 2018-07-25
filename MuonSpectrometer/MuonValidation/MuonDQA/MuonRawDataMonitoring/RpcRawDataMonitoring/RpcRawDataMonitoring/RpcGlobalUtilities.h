/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 global functions and stuff ...
 -----------------------------------------
 ***************************************************************************/
 
#ifndef RPCRAWDATAMONITORING_RPCGLOBALUTILITIES_H
#define RPCRAWDATAMONITORING_RPCGLOBALUTILITIES_H

 
#include <string>

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
 

class RpcIdHelper;
namespace RpcGM 
{
  
  
   
  std::vector<int>	     	 RpcStripShift    (const MuonGM::MuonDetectorManager* m_muonMgr,const RpcIdHelper* m_rpcIdHelper, Identifier prdcoll_id, int  irpctriggerInfo);
  std::vector<std::string>   	 RpcLayerSectorSideName(const RpcIdHelper* m_rpcIdHelper, Identifier prdcoll_id, int  irpctriggerInfo);
    
    
}

#endif // RPCRAWDATAMONITORING_RPCGLOBALUTILITIES_H
