/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewAlgs/IDCCacheCreatorBase.h"
#include "AthViews/View.h"

bool IDCCacheCreatorBase::isInsideView(const EventContext& context) const
{
    const IProxyDict* proxy = Atlas::getExtendedEventContext(context).proxy();
    const SG::View* view = dynamic_cast<const SG::View*>(proxy);
    return view != nullptr;
}

IDCCacheCreatorBase::IDCCacheCreatorBase(const std::string &name,ISvcLocator *pSvcLocator) : 
AthReentrantAlgorithm(name,pSvcLocator)
{
  
}

StatusCode IDCCacheCreatorBase::checkInsideViewOnce(const EventContext& ctx) const
{
  if(!m_disableWarningCheck.test_and_set(std::memory_order_relaxed)){ //Only check once
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
  }
  return StatusCode::SUCCESS;
}
