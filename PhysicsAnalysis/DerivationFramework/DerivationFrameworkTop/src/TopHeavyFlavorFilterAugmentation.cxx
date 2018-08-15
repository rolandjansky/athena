/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h"

namespace DerivationFramework {


TopHeavyFlavorFilterAugmentation::TopHeavyFlavorFilterAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_BfilterTool(""),
  m_BBfilterTool(""),
  m_CfilterTool("")
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("BFilterTool", m_BfilterTool);
    declareProperty("BBFilterTool",m_BBfilterTool);
    declareProperty("CFilterTool", m_CfilterTool);


}



TopHeavyFlavorFilterAugmentation::~TopHeavyFlavorFilterAugmentation(){}



StatusCode TopHeavyFlavorFilterAugmentation::initialize(){

  ATH_MSG_INFO("Initialize " );


  if(m_BfilterTool.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " <<m_BfilterTool);
    return StatusCode::FAILURE;
  }
  if(m_BBfilterTool.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " <<m_BBfilterTool);
    return StatusCode::FAILURE;
  }
  if(m_CfilterTool.retrieve().isFailure()){
    ATH_MSG_ERROR("unable to retrieve filter tool " <<m_CfilterTool);
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

  bool passB =m_BfilterTool->filterDecision();
  bool passBB=m_BBfilterTool->filterDecision();
  bool passC =m_CfilterTool->filterDecision();

  int flag = 0;

  if (passBB) // BB and BFilter non-exclusive: BBFilter is tighter than BFilter
      flag = 1;
  else if (passB)
      flag = 2;
  else if (passC) // CFilter and BFilter are exclusive (B-hadrons have precedence)
      flag = 3;


  static SG::AuxElement::Decorator<int> decoration("TopHeavyFlavorFilterFlag");

  decoration(*eventInfo) = flag;

 return StatusCode::SUCCESS;

}



} /// namespace
