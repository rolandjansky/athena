#include "DFStreamEventSelector.h"

DFStreamEventSelector::DFStreamEventSelector(const std::string &name, ISvcLocator* pSvcLocator):AthService(name,pSvcLocator){}

DFStreamEventSelector::~DFStreamEventSelector(){}

StatusCode DFStreamEventSelector::createContext(EvtContext*& c) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::next(EvtContext& c) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::next(EvtContext& c,int jump) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::previous(EvtContext& c) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::previous(EvtContext& c,int jump) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::last(EvtContext& refContext) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::rewind(EvtContext& c) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::createAddress(const EvtContext& c,IOpaqueAddress*& iop) const{
  return StatusCode::SUCCESS;
}
StatusCode DFStreamEventSelector::releaseContext(EvtContext*&)const{
  return StatusCode::SUCCESS;
}

StatusCode DFStreamEventSelector::resetCriteria(const std::string& cr,Context& c)const{
  return StatusCode::SUCCESS;
}
