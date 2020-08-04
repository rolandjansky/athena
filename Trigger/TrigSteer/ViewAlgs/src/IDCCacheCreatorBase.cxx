
#include "ViewAlgs/IDCCacheCreatorBase.h"
#include "AthViews/View.h"

bool IDCCacheCreatorBase::isInsideView(const EventContext& context) const
{
    const IProxyDict* proxy = Atlas::getExtendedEventContext(context).proxy();
    const SG::View* view = dynamic_cast<const SG::View*>(proxy);
    return view != nullptr;
}

IDCCacheCreatorBase::IDCCacheCreatorBase(const std::string &name,ISvcLocator *pSvcLocator) : 
AthReentrantAlgorithm(name,pSvcLocator),
m_disableWarningCheck(false)
{
  
}

StatusCode IDCCacheCreatorBase::checkInsideViewOnce(const EventContext& ctx) const
{
  if(!m_disableWarningCheck && !m_disableWarningCheck.exchange(true)){ //Only check once
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
  }
  return StatusCode::SUCCESS;
}