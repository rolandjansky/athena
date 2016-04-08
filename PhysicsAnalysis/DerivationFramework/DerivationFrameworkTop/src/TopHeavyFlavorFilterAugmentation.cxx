/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h"

namespace DerivationFramework {


TopHeavyFlavorFilterAugmentation::TopHeavyFlavorFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_filterTool("")
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("FilterTool",m_filterTool);


}



TopHeavyFlavorFilterAugmentation::~TopHeavyFlavorFilterAugmentation(){}



StatusCode TopHeavyFlavorFilterAugmentation::initialize(){

  ATH_MSG_INFO("Initialize " );


  if(m_filterTool.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " <<m_filterTool);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}



StatusCode TopHeavyFlavorFilterAugmentation::finalize(){

  return StatusCode::SUCCESS;

}



StatusCode TopHeavyFlavorFilterAugmentation::addBranches() const{

  const xAOD::EventInfo* eventInfo;

  if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  int flavortype=m_filterTool->filterFlag();


  static SG::AuxElement::Decorator<int> decoration("TopHeavyFlavorFilterFlag");

  decoration(*eventInfo) = flavortype;

 return StatusCode::SUCCESS;

}



} /// namespace
