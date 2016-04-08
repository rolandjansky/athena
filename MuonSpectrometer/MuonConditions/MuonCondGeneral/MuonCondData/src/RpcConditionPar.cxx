/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CLIDSvc/CLASS_DEF.h"

#include <map>
#include <vector>
#include <string>

#include "MuonCondData/RpcConditionPar.h"
#include "MuonCondData/Rpc_ConditionDataContainer.h"


#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

RpcConditionPar::RpcConditionPar()
{ 
  //m_panel        = 0;
  //m_stripid      = 0;
}


RpcConditionPar::~RpcConditionPar()
{ }

void RpcConditionPar::setParametersPanel(std::string info_panel, std::string info_strip)
{
  m_info_panel	= info_panel;
  m_info_strip	= info_strip;
  
} 
