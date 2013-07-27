/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CONDITIONDATACONTAINER_H
#define RPC_CONDITIONDATACONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"

#include <map>
#include <vector>
#include <string>

#include "MuonCondData/RpcConditionPar.h"

  
 
 
typedef std::map<Identifier,RpcConditionPar*> Rpc_ConditionDataContainer;
CLASS_DEF( Rpc_ConditionDataContainer , 1307495046 , 1 )
 
 
typedef std::map<Identifier,RpcConditionPar*>::iterator iRpcConditionMap;



#endif // RPC_CONDITIONDATACONTAINER_H
