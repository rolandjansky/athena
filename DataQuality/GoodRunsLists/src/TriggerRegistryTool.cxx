/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/TriggerRegistryTool.h"

TriggerRegistryTool::TriggerRegistryTool( const std::string& type, const std::string& name, const IInterface* parent ) 
 : AthAlgTool( type, name, parent )
{
  declareInterface<ITriggerRegistryTool>(this);
}


TriggerRegistryTool::~TriggerRegistryTool()
{
}


StatusCode 
TriggerRegistryTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITriggerRegistryTool::interfaceID() )  {
     *ppvIf = (ITriggerRegistryTool*)this;
     addRef();
     return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface( riid, ppvIf );
}


bool 
TriggerRegistryTool::registerTriggerSelector(const TString& name, const TString& regexpr, const std::list<TString>& trigpar)
{
  if (m_registry.find(name)!=m_registry.end()) {
    ATH_MSG_WARNING ("registerTriggerSelector() :: trigger selector with name <" << name << "> already registered. Return false.");
    return false;
  }

  ATH_MSG_DEBUG ("registerTriggerSelector() :: trigger selector with name <" << name << "> registered.");
  m_registry[name] = tvtPair(regexpr,trigpar);
  return true;
}

